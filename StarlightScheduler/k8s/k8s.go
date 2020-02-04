package k8s

import "C"
import (
	"encoding/json"
	"errors"
	"k8s.io/api/core/v1"
	metav1 "k8s.io/apimachinery/pkg/apis/meta/v1"
	"k8s.io/apimachinery/pkg/types"
	"k8s.io/client-go/kubernetes"
	"k8s.io/client-go/tools/clientcmd"
	"starlight-scheduler/core"
	"strings"
)

const (
	K8SNodeIdle     = 0
	K8SNodeUsed     = 1
	K8SNodeUnhealth = 2
	K8SNodeUntaint  = 0
	K8SNodeTaint    = 1
)

// 更新节点时使用的数据结构
type PatchValue struct {
	OP    string      `json:"op"`
	Path  string      `json:"path"`
	Value interface{} `json:"value"`
}

// K8S客户端的数据结构
type K8S struct {
	client *kubernetes.Clientset
}

// 创建新的K8S客户端
func NewK8S(kubeConfig string) (*K8S, error) {
	k8s := &K8S{}
	config, err := clientcmd.BuildConfigFromFlags("", kubeConfig)
	if err != nil {
		return nil, err
	}
	k8s.client, err = kubernetes.NewForConfig(config)
	return k8s, err
}

// 在K8S集群中禁用节点
func (k8s *K8S) DisableNode(node *core.Node) error {
	// 如果节点是 master 不应该被操作
	if node.K8SMaster {
		err := errors.New("should not change state of master")
		return err
	}
	// 节点状态没有发生改变，则无需更新
	if node.NodeState == node.OriNodeState {
		return nil
	}
	node.K8SLabels["state"] = node.NodeReason
	patchData := &[]PatchValue{{OP: "add", Path: "/metadata/labels", Value: &node.K8SLabels}, {OP: "replace", Path: "/spec/taints", Value: []v1.Taint{{Key: "starlight-scheduler", Value: "", Effect: v1.TaintEffectNoExecute}}}}
	data, err := json.Marshal(patchData)
	if err != nil {
		return err
	}
	_, err = k8s.client.CoreV1().Nodes().Patch(node.Name, types.JSONPatchType, data)
	if err != nil {
		return err
	} else {
		node.K8SNodeTaint = K8SNodeTaint
		return nil
	}
}

// 在K8S集群中启用节点
func (k8s *K8S) EnableNode(node *core.Node) error {
	// 如果节点是 master 不应该被操作
	if node.K8SMaster {
		err := errors.New("should not change state of master")
		return err
	}
	// 节点状态没有发生改变，则无需更新
	if node.NodeState == node.OriNodeState {
		return nil
	}
	node.K8SLabels["state"] = node.NodeReason
	patchData := &[]PatchValue{{OP: "add", Path: "/metadata/labels", Value: &node.K8SLabels}, {OP: "replace", Path: "/spec/taints", Value: []v1.Taint{}}}
	data, err := json.Marshal(patchData)
	if err != nil {
		return err
	}
	_, err = k8s.client.CoreV1().Nodes().Patch(node.Name, types.JSONPatchType, data)
	if err != nil {
		return err
	} else {
		node.K8SNodeTaint = K8SNodeUntaint
		return nil
	}
}

// 获取和解析K8S集群的信息
func (k8s *K8S) Parse(nodes *map[string]*core.Node, partitions *map[string]*core.Partition) error {
	nodeList, err := k8s.client.CoreV1().Nodes().List(metav1.ListOptions{})
	if err != nil {
		return err
	}
	podList, err := k8s.client.CoreV1().Pods("").List(metav1.ListOptions{})
	if err != nil {
		return err
	}
	for _, node := range nodeList.Items {
		newNode, ok := (*nodes)[node.Name]
		if !ok {
			newNode = &core.Node{}
			newNode.Name = node.Name
			(*nodes)[node.Name] = newNode
		}
		newNode.K8SLabels = node.Labels
		master := false
		partitionName := ""
		reason := ""
		for label, value := range node.Labels {
			if label == "node-role.kubernetes.io/master" {
				master = true
				break
			} else if label == "partition" {
				partitionName = value
			} else if label == "state" {
				reason = value
			}
		}
		if master {
			newNode.K8SMaster = true
			continue
		}
		newNode.K8SMaster = false
		newNode.K8SNodeReason = reason
		newNode.K8SNodeState = K8SNodeIdle
		for _, nodeCondition := range node.Status.Conditions {
			if nodeCondition.Type == v1.NodeReady {
				if nodeCondition.Status != v1.ConditionTrue {
					newNode.K8SNodeState = K8SNodeUnhealth
				}
				break
			}
		}
		newNode.K8SNodeTaint = K8SNodeUntaint
		for _, taint := range node.Spec.Taints {
			if taint.Effect == v1.TaintEffectNoExecute {
				newNode.K8SNodeTaint = K8SNodeTaint
				break
			}
		}
		if partitionName != "" {
			newNode.K8SPartition = partitionName
			partition, ok := (*partitions)[partitionName]
			if !ok {
				partition = &core.Partition{}
				partition.Name = partitionName
				partition.NodeList = make(map[string]*core.Node)
				partition.IdleNodes = make(map[string]*core.Node)
				partition.AllocatableNodes = make(map[string]*core.Node)
				(*partitions)[partitionName] = partition
			}
		}
	}
	for _, pod := range podList.Items {
		if strings.HasPrefix(pod.Namespace, "kube") || pod.Namespace == "default" {
			continue
		}
		if (*nodes)[pod.Spec.NodeName].K8SNodeState != K8SNodeUnhealth {
			(*nodes)[pod.Spec.NodeName].K8SNodeState = K8SNodeUsed
		}
	}
	return nil
}

// 实时获取节点的状态
func (k8s *K8S) NodeState(nodes *map[string]bool) (string, error) {
	nodeList, err := k8s.client.CoreV1().Nodes().List(metav1.ListOptions{})
	if err != nil {
		return "", err
	}
	state := "\n"
	for _, node := range nodeList.Items {
		_, ok := (*nodes)[node.Name]
		if ok {
			for label, value := range node.Labels {
				if label == "state" {
					state += node.Name + ":" + value + "\n"
					break
				}
			}
		}
	}
	return state, nil
}
