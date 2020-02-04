package core

// 集群节点状态的数据结构
type Node struct {
	Name                   string            // 节点名称
	NodeState              int               // 综合考虑之后的节点状态
	OriNodeState           int               // 原始的节点状态
	NodeReason             string            // 节点状态对应的原因
	SlurmPartition         []string          // 节点所属的SLURM分区
	SlurmSmallestPartition string            // 节点归属于多个SLURM分区时节点数目最少的分区
	SlurmNodeState         int               // 节点在SLURM集群中的状态
	SlurmNodeDrain         int               // 节点在SLURM集群中的启用情况
	SlurmNodeReason        string            // 节点在SLURM集群中禁用的原因
	SlurmMaster            bool              // 节点是否在SLURM集群中为master
	K8SPartition           string            // 节点所属的K8S分区
	K8SNodeState           int               // 节点在K8S集群中的状态
	K8SNodeTaint           int               // 节点在K8S集群中的启用情况
	K8SNodeReason          string            // 节点在K8S集群中禁用的原因
	K8SLabels              map[string]string // 节点在K8S集群里的标签
	K8SMaster              bool              // 节点是否在K8S集群中为master
}
