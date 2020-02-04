package slurm

//#cgo CFLAGS: -Iinclude
//#cgo LDFLAGS: -Llib64 -lslurm_wrapper -lslurm
//#include <stdlib.h>
//#include <slurm/slurm_wrapper.h>
import "C"
import (
	"errors"
	"starlight-scheduler/core"
	"strings"
	"unsafe"
)

const (
	SlurmNodeIdle      = 0
	SlurmNodeUsed      = 1
	SlurmNodeUnhealth  = 2
	SlurmNodeUndrain   = 0
	SlurmNodeDrain     = 1
	SlurmPartitionDown = 0
	SlurmPartitionUp   = 1
)

// SLURM客户端的数据结构
type SLURM struct {
}

// 创建新的SLURM客户端
func NewSLURM() *SLURM {
	return &SLURM{}
}

// 返回操作失败的原因
func errorInfo() error {
	errInfo := C.error_info()
	err := errors.New(C.GoString(errInfo))
	C.free(unsafe.Pointer(errInfo))
	return err
}

// 在SLURM集群中禁用节点
func (s *SLURM) DisableNode(node *core.Node) error {
	// 节点状态没有发生改变，则无需更新
	if node.NodeState == node.OriNodeState {
		return nil
	}
	cNodeNames := C.CString(node.Name)
	cReason := C.CString(node.NodeReason)
	ret := C.drain_nodes(cNodeNames, cReason)
	C.free(unsafe.Pointer(cNodeNames))
	C.free(unsafe.Pointer(cReason))
	if ret != 0 {
		return errorInfo()
	}
	node.SlurmNodeDrain = SlurmNodeDrain
	return nil
}

// 在SLURM集群中启用节点
func (s *SLURM) EnableNode(node *core.Node) error {
	// 节点状态没有发生改变，则无需更新
	if node.NodeState == node.OriNodeState {
		return nil
	}
	cNodeNames := C.CString(node.Name)
	ret := C.undrain_nodes(cNodeNames)
	C.free(unsafe.Pointer(cNodeNames))
	if ret != 0 {
		return errorInfo()
	}
	node.SlurmNodeDrain = SlurmNodeUndrain
	return nil
}

// 获取和解析SLURM集群的信息
func (s *SLURM) Parse(nodes *map[string]*core.Node, partitions *map[string]*core.Partition) error {
	var (
		nodeList      C.node_list_t
		partitionList C.partition_list_t
		jobList       C.job_list_t
	)
	ret := int(C.get_node_list_and_partition_list(&nodeList, &partitionList))
	if ret != 0 {
		return errorInfo()
	}
	count := int(nodeList.count)
	nodeSlice := (*[1 << 30]C.node_t)(unsafe.Pointer(nodeList.nodes))
	for i := 0; i < count; i++ {
		node := &core.Node{}
		if nodeSlice[i].name != nil {
			node.Name = C.GoString(nodeSlice[i].name)
		}
		if nodeSlice[i].partitions != nil {
			node.SlurmPartition = strings.Split(C.GoString(nodeSlice[i].partitions), ",")
		}
		node.SlurmNodeDrain = int(nodeSlice[i].drain)
		node.SlurmNodeState = int(nodeSlice[i].state)
		if nodeSlice[i].reason != nil {
			node.SlurmNodeReason = C.GoString(nodeSlice[i].reason)
		}
		(*nodes)[node.Name] = node
	}
	count = int(partitionList.count)
	partitionSlice := (*[1 << 30]C.partition_t)(unsafe.Pointer(partitionList.partitions))
	for i := 0; i < count; i++ {
		partition := &core.Partition{}
		partition.NodeList = make(map[string]*core.Node)
		partition.IdleNodes = make(map[string]*core.Node)
		partition.AllocatableNodes = make(map[string]*core.Node)
		partition.JobCount = 0
		partition.NeededNodes = 0
		if partitionSlice[i].name != nil {
			partition.Name = C.GoString(partitionSlice[i].name)
		}
		partition.State = int(partitionSlice[i].state)
		(*partitions)[partition.Name] = partition
	}
	C.free_node_list_and_partition_list(&nodeList, &partitionList)
	if core.SLURMNumberQueueingJobForConsider > 0 {
		ret := int(C.get_job_list(&jobList))
		if ret != 0 {
			return errorInfo()
		}
		count = int(jobList.count)
		jobSlice := (*[1 << 30]C.job_t)(unsafe.Pointer(jobList.jobs))
		for i := 0; i < count; i++ {
			if jobSlice[i].partition != nil {
				partition, ok := (*partitions)[C.GoString(jobSlice[i].partition)]
				if ok {
					if partition.JobCount < core.SLURMNumberQueueingJobForConsider {
						if partition.NeededNodes < int(jobSlice[i].nodes) {
							partition.NeededNodes = int(jobSlice[i].nodes)
						}
						partition.JobCount++
					}
				}
			}
		}
		C.free_job_list(&jobList)
	}
	return nil
}
