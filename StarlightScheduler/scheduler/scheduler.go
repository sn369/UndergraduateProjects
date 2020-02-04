package scheduler

import (
	"crypto/rsa"
	"crypto/x509"
	"encoding/json"
	"errors"
	"github.com/dgrijalva/jwt-go"
	"io/ioutil"
	"log"
	"net/http"
	"starlight-scheduler/core"
	"starlight-scheduler/k8s"
	"starlight-scheduler/slurm"
	"strconv"
	"strings"
	"sync"
	"time"
)

var validChar1 [256]bool
var validChar2 [256]bool

func init() {
	for i := '0'; i <= '9'; i++ {
		validChar1[i] = true
		validChar2[i] = true
	}
	for i := 'a'; i <= 'z'; i++ {
		validChar1[i] = true
	}
	for i := 'A'; i <= 'Z'; i++ {
		validChar1[i] = true
	}
	validChar1[','] = true
	validChar2[','] = true
	validChar1['-'] = true
	validChar2['-'] = true
	validChar1['['] = true
	validChar1[']'] = true
}

// 调度器的数据结构
type Scheduler struct {
	publickKey         rsa.PublicKey              // 安全认证公钥
	nodes              map[string]*core.Node      // 集群的节点信息
	slurmPartitions    map[string]*core.Partition // slurm集群的分区信息
	k8sPartitions      map[string]*core.Partition // k8s集群的分区信息
	slurm              *slurm.SLURM               // slurm集群的客户端
	k8s                *k8s.K8S                   // k8s集群的客户端
	freeNodes          int                        // 全部的空闲节点的数目
	slurmIdleNodeRatio int                        // slurm空闲节点的比例
	k8sIdleNodeRatio   int                        // k8s空闲节点的比例
	StopChan           chan bool                  // 调度器退出信号的管道
	stop               bool                       // 退出标志
	mutex              sync.Mutex                 // 调度锁
}

// 创建新的调度器实例
func NewScheduler(kubeConfig, secureKey string) (scheduler *Scheduler, err error) {
	var key *rsa.PrivateKey
	scheduler = &Scheduler{}
	data, err := ioutil.ReadFile(secureKey)
	if err != nil {
		return
	}
	key, err = x509.ParsePKCS1PrivateKey(data)
	if err != nil {
		return
	}
	scheduler.publickKey = key.PublicKey
	scheduler.k8s, err = k8s.NewK8S(kubeConfig)
	if err != nil {
		return
	}
	scheduler.slurm = slurm.NewSLURM()
	scheduler.StopChan = make(chan bool, 1)
	scheduler.nodes = make(map[string]*core.Node)
	scheduler.slurmPartitions = make(map[string]*core.Partition)
	scheduler.k8sPartitions = make(map[string]*core.Partition)
	return
}

// 启动调度器
func (scheduler *Scheduler) Start() {
	for {
		// 调度执行期间禁止外部干预节点状态，以免造成节点状态不一致
		scheduler.mutex.Lock()
		now := time.Now()
		log.Println("开始执行融合调度")
		scheduler.schedule()
		duration := time.Now().Sub(now).Seconds()
		log.Println("融合调度执行完毕，总共花费时间" + strconv.FormatFloat(duration, 'f', 2, 64) + "秒，等待" + strconv.Itoa(core.SchedulingPeriod) + "秒后开始执行下一轮调度")
		scheduler.mutex.Unlock()
		// 等待一段时间之后再继续进行调度
		for i := 0; i < core.SchedulingPeriod; i++ {
			if scheduler.stop {
				scheduler.StopChan <- true
				return
			}
			time.Sleep(time.Second)
		}
	}
}

// 从K8S集群获取节点的状态
func (scheduler *Scheduler) getNodeState(node *core.Node) {
	node.NodeState = 0
	for _, reason := range strings.Split(node.K8SNodeReason, "-") {
		state, ok := core.ReasonToState[reason]
		if ok {
			node.NodeState |= state
		}
	}
	// 节点不能处于多种互斥的状态
	count := 0
	if (node.NodeState & core.ScheduledForSlurm) > 0 {
		count++
		if (node.NodeState & core.MaintainingInSlurm) > 0 {
			count++
		}
	}
	if (node.NodeState & core.PreservedForSlurm) > 0 {
		count++
	}
	if (node.NodeState & core.PreservingForSlurm) > 0 {
		count++
	}
	if (node.NodeState & core.ScheduledForK8S) > 0 {
		count++
		if (node.NodeState & core.MaintainingInK8S) > 0 {
			count++
		}
	}
	if (node.NodeState & core.PreservedForK8S) > 0 {
		count++
	}
	if (node.NodeState & core.PreservingForK8S) > 0 {
		count++
	}
	if (node.NodeState & core.ReclaimResource) > 0 {
		count++
	}
	if (node.NodeState & core.NodeAllocatable) > 0 {
		count++
	}
	if node.NodeState&core.MaintainingInSlurm > 0 {
		if count == 0 {
			count++
		}
	}
	if node.NodeState&core.MaintainingInK8S > 0 {
		if node.NodeState&core.MaintainingInSlurm == 0 && count == 0 {
			count++
		}
	}
	node.OriNodeState = node.NodeState
	if count != 1 {
		// 将节点设置为资源回收状态
		if node.OriNodeState != 0 {
			node.NodeState = node.NodeState&(^core.ScheduledForSlurm)&(^core.PreservedForSlurm)&(^core.PreservingForSlurm)&
				(^core.ScheduledForK8S)&(^core.PreservedForK8S)&(^core.PreservingForK8S)&(^core.NodeAllocatable) |
				core.ReclaimResource
		}
	}
}

// 获取节点所属SLURM分区中节点数目最少的分区
func (scheduler *Scheduler) getSmallestSlurmPartition(node *core.Node) {
	nodes := 1024 * 1024 * 1024
	for _, partitionName := range node.SlurmPartition {
		partition, ok := scheduler.slurmPartitions[partitionName]
		if ok {
			if len(partition.NodeList) < nodes {
				node.SlurmSmallestPartition = partitionName
				nodes = len(partition.NodeList)
			}
		}
	}
}

// 合并节点在两个集群的状态，并更新相应分区的信息
func (scheduler *Scheduler) mergeNodeState() {
	scheduler.freeNodes = 0
	for nodeName, node := range scheduler.nodes {
		// 从K8S集群获取节点的状态
		scheduler.getNodeState(node)
		// 获取节点所属SLURM分区中节点数目最少的分区
		scheduler.getSmallestSlurmPartition(node)
		// 优先处理异常情况(异常状态不应该和正常状态并行进行goto操作)
		if node.SlurmNodeState == slurm.SlurmNodeUnhealth || node.K8SNodeState == k8s.K8SNodeUnhealth {
			if node.SlurmNodeState == slurm.SlurmNodeUnhealth {
				node.NodeState |= core.SlurmUnHealth
			} else {
				node.NodeState = node.NodeState & (^core.SlurmUnHealth)
			}
			if node.K8SNodeState == k8s.K8SNodeUnhealth {
				node.NodeState |= core.K8SUnHealth
			} else {
				node.NodeState = node.NodeState & (^core.K8SUnHealth)
			}
		} else {
			node.NodeState = node.NodeState & (^core.SlurmUnHealth) & (^core.K8SUnHealth)
		}
		// 处理节点没有对应分区的异常状态
		if node.K8SPartition == "" {
			if node.K8SNodeState == k8s.K8SNodeUsed {
				if (node.NodeState & core.PreservingForSlurm) == 0 {
					node.NodeState |= core.PreservingForSlurm
				}
			} else {
				if !node.K8SMaster && (node.NodeState & core.PreservedForSlurm) == 0 {
					node.NodeState |= core.PreservedForSlurm
				}
			}
		}
		if node.SlurmPartition == nil {
			if node.SlurmNodeState == slurm.SlurmNodeUsed {
				if (node.NodeState & (core.PreservingForK8S)) == 0 {
					node.NodeState |= core.PreservingForK8S
				}
			} else {
				if !node.SlurmMaster && ((node.NodeState & (core.PreservedForK8S)) == 0) {
					node.NodeState |= core.PreservedForK8S
				}
			}
		}
		if (node.NodeState & core.MaintainingInSlurm) > 0 {
			goto updatePartition
		}
		if (node.NodeState & core.MaintainingInK8S) > 0 {
			goto updatePartition
		}
		if (node.NodeState&core.NodeAllocatable) > 0 ||
			(node.NodeState&core.PreservedForSlurm) > 0 || (node.NodeState&core.ScheduledForSlurm) > 0 ||
			(node.NodeState&core.PreservedForK8S) > 0 || (node.NodeState&core.ScheduledForK8S) > 0 {
			goto updatePartition
		}
		if (node.NodeState & core.ReclaimResource) > 0 {
			if (node.K8SNodeState == k8s.K8SNodeIdle) && (node.SlurmNodeState == slurm.SlurmNodeIdle) {
				node.NodeState = node.NodeState&(^core.ReclaimResource) | core.NodeAllocatable
			}
			goto updatePartition
		}
		if (node.NodeState & core.PreservingForSlurm) > 0 {
			if node.K8SNodeState == k8s.K8SNodeIdle {
				node.NodeState = node.NodeState&(^core.PreservingForSlurm) | core.PreservedForSlurm
			}
			goto updatePartition
		}
		if (node.NodeState & core.PreservingForK8S) > 0 {
			if node.SlurmNodeState == slurm.SlurmNodeIdle {
				node.NodeState = node.NodeState&(^core.PreservingForK8S) | core.PreservedForK8S
			}
			goto updatePartition
		}
		// master 节点的状态
		node.NodeState = 0
	updatePartition:
		freeNode := 0
		for _, partitionName := range node.SlurmPartition {
			partition, ok := scheduler.slurmPartitions[partitionName]
			if ok {
				partition.NodeList[nodeName] = node
				if (node.NodeState&core.MaintainingInSlurm) == 0 && (node.NodeState&core.SlurmUnHealth) == 0 {
					if (node.NodeState&core.ScheduledForSlurm) > 0 || (node.NodeState&core.PreservedForSlurm) > 0 {
						if node.SlurmNodeState == slurm.SlurmNodeIdle {
							partition.IdleNodes[nodeName] = node
							freeNode = 1
						}
					} else if (node.NodeState & core.NodeAllocatable) > 0 {
						partition.AllocatableNodes[nodeName] = node
						freeNode = 1
					}
				}
			}
		}
		if node.K8SPartition != "" {
			partition, ok := scheduler.k8sPartitions[node.K8SPartition]
			if ok {
				partition.NodeList[nodeName] = node
				if (node.NodeState&core.MaintainingInK8S) == 0 && (node.NodeState&core.K8SUnHealth) == 0 {
					if (node.NodeState&core.ScheduledForK8S) > 0 || (node.NodeState&core.PreservedForK8S) > 0 {
						if node.K8SNodeState == k8s.K8SNodeIdle {
							partition.IdleNodes[nodeName] = node
							freeNode = 1
						}
					} else if (node.NodeState & core.NodeAllocatable) > 0 {
						partition.AllocatableNodes[nodeName] = node
						freeNode = 1
					}
				}
			}
		}
		scheduler.freeNodes += freeNode
	}
}

// 计算各类节点的比例分配
func (scheduler *Scheduler) calculateNodeRatio() {
	// 总的节点数目
	totalNodes := len(scheduler.nodes)
	// 计算期望的SLURM和K8S空闲节点的比例
	scheduler.slurmIdleNodeRatio = core.SlurmIdleNodeRatio * scheduler.freeNodes * (100 - core.AllocatableNodeRatio) / (totalNodes * 100)
	scheduler.k8sIdleNodeRatio = core.K8SIdleNodeRatio * scheduler.freeNodes * (100 - core.AllocatableNodeRatio) / (totalNodes * 100)
}

// 设置节点状态信息
func (scheduler *Scheduler) setNodeState(node *core.Node) {
	node.NodeReason = ""
	if node.K8SMaster || node.SlurmMaster {
		return
	}
	state := 1
	for state < core.NodeStateEnd {
		if (node.NodeState & state) > 0 {
			node.NodeReason += core.StateToReason[state] + "-"
		}
		state <<= 1
	}
	node.NodeReason = node.NodeReason[:len(node.NodeReason)-1]
}

// 检查节点状态一致性，并将节点状态更新到SLURM与K8S集群中
func (scheduler *Scheduler) updateNodeState() {
	for nodeName, node := range scheduler.nodes {
		// 节点状态没有发生改变，则无需更新
		if node.NodeState == node.OriNodeState {
			continue
		}
		scheduler.setNodeState(node)
		log.Printf("nodemname = %s, NodeState = %d, OriNodeState = %d\n", nodeName, node.NodeState, node.OriNodeState)
		updateK8S := false
		updateSlurm := false
		success := true
		if (node.NodeState&core.ReclaimResource) > 0 || (node.NodeState&core.NodeAllocatable) > 0 {
			err := scheduler.slurm.DisableNode(node)
			if err != nil {
				log.Println(err)
				success = false
			} else {
				updateSlurm = true
			}
			err = scheduler.k8s.DisableNode(node)
			if err != nil {
				log.Println(err)
				success = false
			} else {
				updateK8S = true
			}
		}
		if (node.NodeState&core.SlurmUnHealth) > 0 || (node.NodeState&core.MaintainingInSlurm) > 0 {
			if !updateSlurm {
				err := scheduler.slurm.DisableNode(node)
				if err != nil {
					log.Println(err)
					success = false
				} else {
					updateSlurm = true
				}
			}
			if !updateK8S {
				var err error
				if node.K8SNodeTaint == k8s.K8SNodeTaint {
					err = scheduler.k8s.DisableNode(node)
				} else {
					node.NodeState |= core.ScheduledForK8S
					err = scheduler.k8s.EnableNode(node)
				}
				if err != nil {
					log.Println(err)
					success = false
				} else {
					updateK8S = true
				}
			}
		} else if (node.NodeState&core.ScheduledForSlurm) > 0 || (node.NodeState&core.PreservedForSlurm) > 0 {
			if !updateSlurm {
				err := scheduler.slurm.EnableNode(node)
				if err != nil {
					log.Println(err)
					success = false
				} else {
					updateSlurm = true
				}
			}
			if !updateK8S {
				err := scheduler.k8s.DisableNode(node)
				if err != nil {
					log.Println(err)
					success = false
				} else {
					updateK8S = true
				}
			}
		} else if (node.NodeState & core.PreservingForSlurm) > 0 {
			if !updateSlurm {
				err := scheduler.slurm.DisableNode(node)
				if err != nil {
					log.Println(err)
					success = false
				} else {
					updateSlurm = true
				}
			}
			if !updateK8S {
				err := scheduler.k8s.DisableNode(node)
				if err != nil {
					log.Println(err)
					success = false
				} else {
					updateK8S = true
				}
			}
		}
		if (node.NodeState&core.K8SUnHealth) > 0 || (node.NodeState&core.MaintainingInK8S) > 0 {
			if !updateK8S {
				err := scheduler.k8s.DisableNode(node)
				if err != nil {
					log.Println(err)
					success = false
				} else {
					updateK8S = true
				}
			}
			if !updateSlurm {
				var err error
				if node.SlurmNodeDrain == slurm.SlurmNodeDrain {
					err = scheduler.slurm.DisableNode(node)
					if err != nil {
						log.Println(err)
						success = false
					} else {
						updateSlurm = true
					}
				}
			}
		} else if (node.NodeState&core.ScheduledForK8S) > 0 || (node.NodeState&core.PreservedForK8S) > 0 {
			if !updateK8S {
				err := scheduler.k8s.EnableNode(node)
				if err != nil {
					log.Println(err)
				}
				updateK8S = true
			}
			if !updateSlurm {
				err := scheduler.slurm.DisableNode(node)
				if err != nil {
					log.Println(err)
					success = false
				} else {
					updateSlurm = true
				}
			}
		} else if (node.NodeState & core.PreservingForK8S) > 0 {
			if !updateK8S {
				err := scheduler.k8s.DisableNode(node)
				if err != nil {
					log.Println(err)
					success = false
				} else {
					updateK8S = true
				}
			}
			if !updateSlurm {
				err := scheduler.slurm.DisableNode(node)
				if err != nil {
					log.Println(err)
					success = false
				} else {
					updateSlurm = true
				}
			}
		}
		if success {
			node.OriNodeState = node.NodeState
		}
	}
}

// 对K8S分区的节点状态进行调整
func (scheduler *Scheduler) adjustK8S() {
	var idleNodes int
	for _, partition := range scheduler.k8sPartitions {
		if core.BalanceGlobal {
			totalNodes := len(partition.NodeList)
			idleNodes = totalNodes * scheduler.k8sIdleNodeRatio / 100
		} else {
			freeNodes := len(partition.IdleNodes) + len(partition.AllocatableNodes)
			idleNodes = freeNodes * (100 - core.AllocatableNodeRatio) * core.K8SIdleNodeRatio / 10000
		}
		if idleNodes < 1 {
			idleNodes = 1
		}
		if len(partition.IdleNodes) < idleNodes {
			// 空闲节点不足，需要进行补充
			log.Println("开始往K8S分区" + partition.Name + "补充节点")
			need := idleNodes - len(partition.IdleNodes)
			count := 0
			for _, node := range partition.AllocatableNodes {
				if (node.NodeState & core.NodeAllocatable) > 0 && node.NodeState & core.MaintainingInK8S == 0 {
					node.NodeState = node.NodeState&(^core.NodeAllocatable) | core.ScheduledForK8S
					count++
					if count == need {
						break
					}
				}
			}
			log.Println("完成往K8S分区" + partition.Name + "补充节点，期望补充" + strconv.Itoa(need) + "个，实际补充" + strconv.Itoa(count) + "个")
		} else if len(partition.IdleNodes) > idleNodes {
			// 剔除多余的空闲节点
			log.Println("开始在K8S分区" + partition.Name + "剔除节点")
			need := len(partition.IdleNodes) - idleNodes
			count := 0
			for _, node := range partition.IdleNodes {
				if (node.NodeState & core.ScheduledForK8S) > 0 {
					node.NodeState = node.NodeState&(^core.ScheduledForK8S) | core.ReclaimResource
					count++
					if count == need {
						break
					}
				}
			}
			log.Println("完成在K8S分区" + partition.Name + "剔除节点，期望剔除" + strconv.Itoa(need) + "个，实际剔除" + strconv.Itoa(count) + "个")
		}
	}
}

// 对SLURM分区的节点状态进行调整
func (scheduler *Scheduler) adjustSLURM() {
	var idleNodes int
	for _, partition := range scheduler.slurmPartitions {
		if partition.State == slurm.SlurmPartitionUp {
			if core.BalanceGlobal {
				totalNodes := len(partition.NodeList)
				idleNodes = totalNodes * scheduler.slurmIdleNodeRatio / 100
			} else {
				freeNodes := len(partition.IdleNodes) + len(partition.AllocatableNodes)
				idleNodes = freeNodes * (100 - core.AllocatableNodeRatio) * core.SlurmIdleNodeRatio / 10000
			}
			if idleNodes < partition.NeededNodes {
				idleNodes = partition.NeededNodes
			}
			if idleNodes < 1 {
				idleNodes = 1
			}
			if len(partition.IdleNodes) < idleNodes {
				// 空闲节点不足，需要进行补充
				log.Println("开始往SLURM分区" + partition.Name + "补充节点")
				need := idleNodes - len(partition.IdleNodes)
				count := 0
				for _, node := range partition.AllocatableNodes {
					// 当节点归属于多个分区时由最小的分区来决定是否补充节点
					if (node.NodeState&core.NodeAllocatable) > 0 && (partition.Name == node.SlurmSmallestPartition) &&
						(node.NodeState & core.MaintainingInSlurm) == 0 {
						node.NodeState = node.NodeState&(^core.NodeAllocatable) | core.ScheduledForSlurm
						count++
						if count == need {
							break
						}
					}
				}
				log.Println("完成往SLURM分区" + partition.Name + "补充节点，期望补充" + strconv.Itoa(need) + "个，实际补充" + strconv.Itoa(count) + "个")
			} else if len(partition.IdleNodes) > idleNodes {
				// 剔除多余的空闲节点
				log.Println("开始在SLURM分区" + partition.Name + "剔除节点")
				need := len(partition.IdleNodes) - idleNodes
				count := 0
				for _, node := range partition.IdleNodes {
					// 当节点归属于多个分区时由最小的分区来决定是否剔除节点
					if (node.NodeState&core.ScheduledForSlurm) > 0 && (partition.Name == node.SlurmSmallestPartition) {
						node.NodeState = node.NodeState&(^core.ScheduledForSlurm) | core.ReclaimResource
						count++
						if count == need {
							break
						}
					}
				}
				log.Println("完成在SLURM分区" + partition.Name + "剔除节点，期望剔除" + strconv.Itoa(need) + "个，实际剔除" + strconv.Itoa(count) + "个")
			}
		}
	}
}

// 执行调度操作
func (scheduler *Scheduler) schedule() {
	// 首先获取和解析SLURM集群的信息
	log.Println("开始获取和解析SLURM集群的信息")
	err := scheduler.slurm.Parse(&scheduler.nodes, &scheduler.slurmPartitions)
	if err != nil {
		log.Println(err)
		return
	}
	log.Println("SLURM集群的信息获取和解析完毕")
	// 接下来获取和解析K8S集群的信息
	log.Println("开始获取和解析K8S集群的信息")
	err = scheduler.k8s.Parse(&scheduler.nodes, &scheduler.k8sPartitions)
	if err != nil {
		log.Println(err)
		return
	}
	log.Println("K8S集群的信息获取和解析完毕")
	// 合并节点在两个集群的状态，并更新相应分区的信息
	log.Println("开始合并节点在两个集群中的状态")
	scheduler.mergeNodeState()
	log.Println("节点在两个集群中的状态合并完毕")
	// 计算各类节点的比例分配
	log.Println("开始计算各类节点的比例分配")
	scheduler.calculateNodeRatio()
	log.Println("各类节点的比例分配计算完毕")
	// 对K8S分区的节点状态进行调整
	log.Println("开始对K8S分区的节点状态进行调整")
	scheduler.adjustK8S()
	log.Println("K8S分区的节点状态调整完毕")
	// 对SLURM分区的节点状态进行调整
	log.Println("开始对SLURM分区的节点状态进行调整")
	scheduler.adjustSLURM()
	log.Println("SLURM分区的节点状态调整完毕")
	// 检查节点状态一致性，并将节点状态更新到SLURM与K8S集群中
	log.Println("开始将节点状态更新到SLURM与K8S集群中")
	scheduler.updateNodeState()
	log.Println("完成将节点状态更新到SLURM与K8S集群中")
}

// 停止调度器
func (scheduler *Scheduler) Stop() {
	scheduler.stop = true
}

// 验证请求
func (scheduler *Scheduler) verify(r *http.Request) error {
	tokenStr := r.Header.Get(core.TokenName)
	token, err := jwt.ParseWithClaims(tokenStr, &core.Token{}, func(_ *jwt.Token) (interface{}, error) {
		return &scheduler.publickKey, nil
	})
	if err != nil {
		return err
	}
	if token.Claims.(*core.Token).Name != core.TokenName {
		return errors.New("token名不正确")
	}
	return nil
}

// 检查节点列表是否包含非法字符
func (scheduler *Scheduler) validNodeNames(nodeNames string) (string, string, error) {
	c1 := 0    // 统计'['的数目
	c2 := 0    // 统计']'的数目
	c1Idx := 0 // 最早一个'['的索引
	c2Idx := 0 // 最后一个']'的索引
	for i := 0; i < len(nodeNames); i++ {
		if !validChar1[nodeNames[i]] {
			return "", "", errors.New("节点列表存在非法字符")
		}
		if nodeNames[i] == '[' {
			if c1 == 0 {
				c1Idx = i
			}
			c1++
		} else if nodeNames[i] == ']' {
			c2++
			c2Idx = i
		}
	}
	if c1 == 0 && c2 == 0 { // 不存在中括号
		return nodeNames, "", nil
	}
	if c1 != 1 || c2 != 1 || (c1Idx+1) >= c2Idx {
		return "", "", errors.New("节点列表中'['和']'的数目或者位置不正确")
	}
	rest := nodeNames[c1Idx+1 : c2Idx]
	for i := 0; i < len(rest); i++ {
		if !validChar2[rest[i]] {
			return "", "", errors.New("节点列表中括号内存在非法字符")
		}
	}
	if rest[0] == ',' || rest[0] == '-' || rest[len(rest)-1] == ',' || rest[len(rest)-1] == '-' {
		return "", "", errors.New("节点列表中括号内','或'-'位置不正确")
	}
	repeat := false // 避免形如[xx-yy-zz]的出现
	near := false   // 避免连续的','和'-'出现，如",,"或者",-"
	for i := 0; i < len(rest); i++ {
		if rest[i] == ',' {
			if near {
				return "", "", errors.New("节点列表中括号内','或'-'位置不正确")
			}
			repeat = false
			near = true
		} else if rest[i] == '-' {
			if near {
				return "", "", errors.New("节点列表中括号内','或'-'位置不正确")
			}
			if repeat {
				return "", "", errors.New("节点列表中括号内','或'-'位置不正确")
			}
			repeat = true
			near = true
		} else {
			near = false
		}
	}
	return nodeNames[:c1Idx], rest, nil
}

// 将节点名解析成节点列表
func (scheduler *Scheduler) parseNodeList(nodeNames string) (*map[string]bool, error) {
	nodes := make(map[string]bool)
	baseName, rest, err := scheduler.validNodeNames(nodeNames)
	if err != nil {
		return nil, err
	}
	if rest == "" {
		nodes[baseName] = true
		return &nodes, nil
	}
	start := 0
	for i := 0; i < len(rest); i++ {
		if rest[i] == ',' {
			nodes[baseName+strconv.Itoa(start)] = true
			start = 0
		} else if rest[i] == '-' {
			end := 0
			for j := i + 1; j < len(rest); j++ {
				if rest[j] == ',' {
					for k := start; k <= end; k++ {
						nodes[baseName+strconv.Itoa(k)] = true
					}
					i, start = j, 0
					break
				} else {
					end = end*10 + int(rest[j]-'0')
				}
			}
		} else {
			start = start*10 + int(rest[i]-'0')
			if i == (len(rest) - 1) {
				nodes[baseName+strconv.Itoa(start)] = true
			}
		}
	}
	return &nodes, nil
}

// 在集群中保留节点
func (scheduler *Scheduler) preserveNode(w http.ResponseWriter, r *http.Request) {
	var (
		response core.Response
		err      error
	)
	defer func() {
		if err != nil {
			response.Err = err.Error()
		}
		err = json.NewEncoder(w).Encode(&response)
		if err != nil {
			log.Println(err)
		}
	}()
	err = scheduler.verify(r)
	if err != nil {
		return
	}
	request := &struct {
		NodeNames string
		Cluster   string
	}{}
	err = json.NewDecoder(r.Body).Decode(&request)
	if err != nil {
		return
	}
	nodes, err := scheduler.parseNodeList(request.NodeNames)
	if err != nil {
		return
	}
	if request.Cluster != "slurm" && request.Cluster != "k8s" {
		err = errors.New("指定的集群类型不存在")
		return
	}
	errStr := ""
	scheduler.mutex.Lock()
	defer scheduler.mutex.Unlock()
	for nodeName := range *nodes {
		node, ok := scheduler.nodes[nodeName]
		if ok {
			if request.Cluster == "slurm" {
				if (node.NodeState & core.MaintainingInSlurm) > 0 {
					errStr += "can not preserve " + nodeName + ";"
					continue
				}
				if (node.NodeState & core.ScheduledForSlurm) > 0 {
					node.NodeState = node.NodeState&(^core.ScheduledForSlurm) | core.PreservedForSlurm
					scheduler.setNodeState(node)
					err = scheduler.k8s.DisableNode(node)
					if err != nil {
						errStr += err.Error()
					}
				} else if (node.NodeState&core.ScheduledForK8S) > 0 || (node.NodeState&core.PreservingForK8S) > 0 || (node.NodeState&core.PreservedForK8S) > 0 {
					node.NodeState = node.NodeState&(^core.ScheduledForK8S)&(^core.PreservingForK8S)&(^core.PreservedForK8S) | core.PreservingForSlurm
					scheduler.setNodeState(node)
					err = scheduler.k8s.DisableNode(node)
					if err != nil {
						errStr += err.Error()
					}
					err = scheduler.slurm.DisableNode(node)
					if err != nil {
						errStr += err.Error()
					}
				} else if (node.NodeState & core.ReclaimResource) > 0 {
					node.NodeState = node.NodeState&(^core.ReclaimResource) | core.PreservingForSlurm
					scheduler.setNodeState(node)
					err = scheduler.k8s.DisableNode(node)
					if err != nil {
						errStr += err.Error()
					}
					err = scheduler.slurm.DisableNode(node)
					if err != nil {
						errStr += err.Error()
					}
				} else if (node.NodeState & core.NodeAllocatable) > 0 {
					node.NodeState = node.NodeState&(^core.NodeAllocatable) | core.PreservedForSlurm
					scheduler.setNodeState(node)
					err = scheduler.k8s.DisableNode(node)
					if err != nil {
						errStr += err.Error()
					}
					err = scheduler.slurm.EnableNode(node)
					if err != nil {
						errStr += err.Error()
					}
				} else {
					node.NodeState = node.NodeState | core.PreservedForSlurm
					scheduler.setNodeState(node)
					err = scheduler.k8s.DisableNode(node)
					if err != nil {
						errStr += err.Error()
					}
					err = scheduler.slurm.EnableNode(node)
					if err != nil {
						errStr += err.Error()
					}
				}
			} else {
				if (node.NodeState & core.MaintainingInK8S) > 0 {
					errStr += "can not preserve " + nodeName + ";"
					continue
				}
				if (node.NodeState & core.ScheduledForK8S) > 0 {
					node.NodeState = node.NodeState&(^core.ScheduledForK8S) | core.PreservedForK8S
					scheduler.setNodeState(node)
					err = scheduler.slurm.DisableNode(node)
					if err != nil {
						errStr += err.Error()
					}
					err = scheduler.k8s.EnableNode(node)
					if err != nil {
						errStr += err.Error()
					}
				} else if (node.NodeState&core.ScheduledForSlurm) > 0 || (node.NodeState&core.PreservingForSlurm) > 0 || (node.NodeState&core.PreservedForSlurm) > 0 {
					node.NodeState = node.NodeState&(^core.ScheduledForSlurm)&(^core.PreservingForSlurm)&(^core.PreservedForSlurm) | core.PreservingForK8S
					scheduler.setNodeState(node)
					err = scheduler.k8s.DisableNode(node)
					if err != nil {
						errStr += err.Error()
					}
					err = scheduler.slurm.DisableNode(node)
					if err != nil {
						errStr += err.Error()
					}
				} else if (node.NodeState & core.ReclaimResource) > 0 {
					node.NodeState = node.NodeState&(^core.ReclaimResource) | core.PreservingForK8S
					scheduler.setNodeState(node)
					err = scheduler.k8s.DisableNode(node)
					if err != nil {
						errStr += err.Error()
					}
					err = scheduler.slurm.DisableNode(node)
					if err != nil {
						errStr += err.Error()
					}
				} else if (node.NodeState & core.NodeAllocatable) > 0 {
					node.NodeState = node.NodeState&(^core.NodeAllocatable) | core.PreservedForK8S
					scheduler.setNodeState(node)
					err = scheduler.k8s.EnableNode(node)
					if err != nil {
						errStr += err.Error()
					}
					err = scheduler.slurm.DisableNode(node)
					if err != nil {
						errStr += err.Error()
					}
				} else if (node.NodeState & core.MaintainingInK8S) > 0 {
					node.NodeState = node.NodeState | core.PreservedForK8S
					scheduler.setNodeState(node)
					err = scheduler.k8s.DisableNode(node)
					if err != nil {
						errStr += err.Error()
					}
					err = scheduler.slurm.DisableNode(node)
					if err != nil {
						errStr += err.Error()
					}
				} else {
					node.NodeState = node.NodeState | core.PreservedForK8S
					scheduler.setNodeState(node)
					err = scheduler.k8s.EnableNode(node)
					if err != nil {
						errStr += err.Error()
					}
					err = scheduler.slurm.DisableNode(node)
					if err != nil {
						errStr += err.Error()
					}
				}
			}
		} else {
			errStr += "节点" + nodeName + "不存在;"
		}
	}
	if errStr != "" {
		err = errors.New(errStr)
	} else {
		response.Spec = "节点" + request.NodeNames + "在" + request.Cluster + "集群中保留成功"
	}
}

// 解除节点的保留状态
func (scheduler *Scheduler) unPreserveNode(w http.ResponseWriter, r *http.Request) {
	var (
		response core.Response
		err      error
	)
	defer func() {
		if err != nil {
			response.Err = err.Error()
		}
		err = json.NewEncoder(w).Encode(&response)
		if err != nil {
			log.Println(err)
		}
	}()
	err = scheduler.verify(r)
	if err != nil {
		return
	}
	request := &struct {
		NodeNames string
		Cluster   string
	}{}
	err = json.NewDecoder(r.Body).Decode(&request)
	if err != nil {
		return
	}
	nodes, err := scheduler.parseNodeList(request.NodeNames)
	if err != nil {
		return
	}
	if request.Cluster != "slurm" && request.Cluster != "k8s" {
		err = errors.New("指定的集群类型不存在")
		return
	}
	errStr := ""
	scheduler.mutex.Lock()
	defer scheduler.mutex.Unlock()
	for nodeName := range *nodes {
		node, ok := scheduler.nodes[nodeName]
		if ok {
			if request.Cluster == "slurm" {
				if (node.NodeState & core.PreservedForSlurm) > 0 {
					if (node.NodeState & core.MaintainingInSlurm) == 0 {
						node.NodeState = node.NodeState&(^core.PreservedForSlurm) | core.ScheduledForSlurm
						scheduler.setNodeState(node)
						err = scheduler.slurm.EnableNode(node)
						if err != nil {
							errStr += err.Error()
						}
					} else {
						node.NodeState = node.NodeState & (^core.PreservedForSlurm)
						scheduler.setNodeState(node)
						err = scheduler.slurm.DisableNode(node)
						if err != nil {
							errStr += err.Error()
						}
					}
					err = scheduler.k8s.DisableNode(node)
					if err != nil {
						errStr += err.Error()
					}
				} else if (node.NodeState & core.PreservingForSlurm) > 0 {
					node.NodeState = node.NodeState&(^core.PreservingForSlurm) | core.ReclaimResource
					scheduler.setNodeState(node)
					err = scheduler.slurm.DisableNode(node)
					if err != nil {
						errStr += err.Error()
					}
					err = scheduler.k8s.DisableNode(node)
					if err != nil {
						errStr += err.Error()
					}
				}
			} else {
				if (node.NodeState & core.PreservedForK8S) > 0 {
					if (node.NodeState & core.MaintainingInK8S) == 0 {
						node.NodeState = node.NodeState&(^core.PreservedForK8S) | core.ScheduledForK8S
						scheduler.setNodeState(node)
						err = scheduler.k8s.EnableNode(node)
						if err != nil {
							errStr += err.Error()
						}
					} else {
						node.NodeState = node.NodeState & (^core.PreservedForK8S)
						scheduler.setNodeState(node)
						err = scheduler.k8s.DisableNode(node)
						if err != nil {
							errStr += err.Error()
						}
					}
					scheduler.setNodeState(node)
					err = scheduler.slurm.DisableNode(node)
					if err != nil {
						errStr += err.Error()
					}
				} else if (node.NodeState & core.PreservingForK8S) > 0 {
					node.NodeState = node.NodeState&(^core.PreservingForK8S) | core.ReclaimResource
					scheduler.setNodeState(node)
					err = scheduler.slurm.DisableNode(node)
					if err != nil {
						errStr += err.Error()
					}
					err = scheduler.k8s.DisableNode(node)
					if err != nil {
						errStr += err.Error()
					}
				}
			}
		} else {
			errStr += "节点" + nodeName + "不存在;"
		}
	}
	if errStr != "" {
		err = errors.New(errStr)
	} else {
		response.Spec = "节点" + request.NodeNames + "在" + request.Cluster + "集群中解除保留成功"
	}
}

// 在集群中维护节点
func (scheduler *Scheduler) maintainingNode(w http.ResponseWriter, r *http.Request) {
	var (
		response core.Response
		err      error
	)
	defer func() {
		if err != nil {
			response.Err = err.Error()
		}
		err = json.NewEncoder(w).Encode(&response)
		if err != nil {
			log.Println(err)
		}
	}()
	err = scheduler.verify(r)
	if err != nil {
		return
	}
	request := &struct {
		NodeNames string
		Cluster   string
	}{}
	err = json.NewDecoder(r.Body).Decode(&request)
	if err != nil {
		return
	}
	nodes, err := scheduler.parseNodeList(request.NodeNames)
	if err != nil {
		return
	}
	if request.Cluster != "slurm" && request.Cluster != "k8s" {
		err = errors.New("指定的集群类型不存在")
		return
	}
	errStr := ""
	scheduler.mutex.Lock()
	defer scheduler.mutex.Unlock()
	for nodeName := range *nodes {
		node, ok := scheduler.nodes[nodeName]
		if ok {
			if request.Cluster == "slurm" {
				if (node.NodeState & core.MaintainingInSlurm) == 0 {
					// 为了在slurm维护时尽可能给k8s使用，需要将ScheduledForSlurm去除
					if (node.NodeState & core.ScheduledForSlurm) > 0 {
						node.NodeState = node.NodeState&(^core.ScheduledForSlurm) | core.MaintainingInSlurm
					} else {
						node.NodeState = node.NodeState | core.MaintainingInSlurm
					}
					scheduler.setNodeState(node)
					err = scheduler.slurm.DisableNode(node)
					if err != nil {
						errStr += err.Error()
					}
					if node.K8SNodeTaint == k8s.K8SNodeTaint {
						err = scheduler.k8s.DisableNode(node)
						if err != nil {
							errStr += err.Error()
						}
					} else {
						node.NodeState |= core.ScheduledForK8S
						err = scheduler.k8s.EnableNode(node)
						if err != nil {
							errStr += err.Error()
						}
					}
				}
			} else {
				if (node.NodeState & core.MaintainingInK8S) == 0 {
					// 为了在k8s维护时尽可能给slurm使用，需要将ScheduledForK8S去除
					if (node.NodeState & core.ScheduledForK8S) > 0 {
						node.NodeState = node.NodeState&(^core.ScheduledForK8S) | core.MaintainingInK8S
					} else {
						node.NodeState = node.NodeState | core.MaintainingInK8S
					}
					scheduler.setNodeState(node)
					if node.SlurmNodeDrain == slurm.SlurmNodeDrain {
						err = scheduler.slurm.DisableNode(node)
						if err != nil {
							errStr += err.Error()
						}
					} else {
						node.NodeState |= core.ScheduledForSlurm
						err = scheduler.slurm.EnableNode(node)
						if err != nil {
							errStr += err.Error()
						}
					}
					err = scheduler.k8s.DisableNode(node)
					if err != nil {
						errStr += err.Error()
					}
				}
			}
		} else {
			errStr += "节点" + nodeName + "不存在;"
		}
	}
	if errStr != "" {
		err = errors.New(errStr)
	} else {
		response.Spec = "节点" + request.NodeNames + "在" + request.Cluster + "集群中设置维护成功"
	}
}

// 解除节点的维护状态
func (scheduler *Scheduler) unMaintainingNode(w http.ResponseWriter, r *http.Request) {
	var (
		response core.Response
		err      error
	)
	defer func() {
		if err != nil {
			response.Err = err.Error()
		}
		err = json.NewEncoder(w).Encode(&response)
		if err != nil {
			log.Println(err)
		}
	}()
	err = scheduler.verify(r)
	if err != nil {
		return
	}
	request := &struct {
		NodeNames string
		Cluster   string
	}{}
	err = json.NewDecoder(r.Body).Decode(&request)
	if err != nil {
		return
	}
	nodes, err := scheduler.parseNodeList(request.NodeNames)
	if err != nil {
		return
	}
	if request.Cluster != "slurm" && request.Cluster != "k8s" {
		err = errors.New("指定的集群类型不存在")
		return
	}
	errStr := ""
	scheduler.mutex.Lock()
	defer scheduler.mutex.Unlock()
	for nodeName := range *nodes {
		node, ok := scheduler.nodes[nodeName]
		if ok {
			if request.Cluster == "slurm" {
				if (node.NodeState & core.MaintainingInSlurm) > 0 {
					if node.NodeState&core.NodeAllocatable == 0 && node.NodeState&core.ScheduledForK8S == 0 &&
						node.NodeState&core.PreservingForK8S == 0 && node.NodeState&core.PreservedForK8S == 0 &&
						node.NodeState&core.PreservingForSlurm == 0 && node.NodeState&core.PreservedForSlurm == 0 &&
						node.NodeState&core.ReclaimResource == 0 {
						node.NodeState = node.NodeState&(^core.MaintainingInSlurm) | core.ReclaimResource
					} else {
						node.NodeState = node.NodeState & (^core.MaintainingInSlurm)
					}
					scheduler.setNodeState(node)
					if node.SlurmNodeDrain == slurm.SlurmNodeDrain {
						err = scheduler.slurm.DisableNode(node)
						if err != nil {
							errStr += err.Error()
						}
					}
					if node.K8SNodeTaint == k8s.K8SNodeTaint {
						err = scheduler.k8s.DisableNode(node)
						if err != nil {
							errStr += err.Error()
						}
					} else {
						err = scheduler.k8s.EnableNode(node)
						if err != nil {
							errStr += err.Error()
						}
					}
				} else {
					errStr += "节点" + nodeName + "并未在slurm中维护;"
				}
			} else {
				if (node.NodeState & core.MaintainingInK8S) > 0 {
					if node.NodeState&core.NodeAllocatable == 0 && node.NodeState&core.ScheduledForSlurm == 0 &&
						node.NodeState&core.PreservingForSlurm == 0 && node.NodeState&core.PreservedForSlurm == 0 &&
						node.NodeState&core.PreservingForK8S == 0 && node.NodeState&core.PreservedForK8S == 0 &&
						node.NodeState&core.ReclaimResource == 0 {
						node.NodeState = node.NodeState&(^core.MaintainingInK8S) | core.ReclaimResource
					} else {
						node.NodeState = node.NodeState & (^core.MaintainingInK8S)
					}
					scheduler.setNodeState(node)
					if node.SlurmNodeDrain == slurm.SlurmNodeDrain {
						err = scheduler.slurm.DisableNode(node)
						if err != nil {
							errStr += err.Error()
						}
					}
					if node.K8SNodeTaint == k8s.K8SNodeTaint {
						err = scheduler.k8s.DisableNode(node)
						if err != nil {
							errStr += err.Error()
						}
					} else {
						err = scheduler.k8s.EnableNode(node)
						if err != nil {
							errStr += err.Error()
						}
					}
				} else {
					errStr += "节点" + nodeName + "并未在k8s中维护;"
				}
			}
		} else {
			errStr += "节点" + nodeName + "不存在;"
		}
	}
	if errStr != "" {
		err = errors.New(errStr)
	} else {
		response.Spec = "节点" + request.NodeNames + "在" + request.Cluster + "集群中解除维护成功"
	}
}

// 实时获取节点的状态
func (scheduler *Scheduler) nodeState(w http.ResponseWriter, r *http.Request) {
	var (
		response core.Response
		err      error
		nodes    *map[string]bool
	)
	defer func() {
		if err != nil {
			response.Err = err.Error()
		}
		err = json.NewEncoder(w).Encode(&response)
		if err != nil {
			log.Println(err)
		}
	}()
	err = scheduler.verify(r)
	if err != nil {
		return
	}
	request := &struct {
		NodeNames string
	}{}
	err = json.NewDecoder(r.Body).Decode(&request)
	if err != nil {
		return
	}

	if request.NodeNames != "" {
		nodes, err = scheduler.parseNodeList(request.NodeNames)
		if err != nil {
			return
		}
	} else {
		temp := make(map[string]bool)
		for key := range scheduler.nodes {
			temp[key] = true
		}
		nodes = &temp
	}
	scheduler.mutex.Lock()
	response.Spec, err = scheduler.k8s.NodeState(nodes)
	scheduler.mutex.Unlock()

}

// 返回HTTP路由
func (scheduler *Scheduler) Router() http.Handler {
	mux := &http.ServeMux{}
	mux.HandleFunc("/node/preserve", scheduler.preserveNode)
	mux.HandleFunc("/node/unPreserve", scheduler.unPreserveNode)
	mux.HandleFunc("/node/maintaining", scheduler.maintainingNode)
	mux.HandleFunc("/node/unMaintaining", scheduler.unMaintainingNode)
	mux.HandleFunc("/node/state", scheduler.nodeState)
	return mux
}
