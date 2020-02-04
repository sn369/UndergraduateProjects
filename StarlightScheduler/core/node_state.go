package core

const (
	// 设置节点在SLURM集群处于维护状态
	MaintainingInSlurm = 1 << iota
	// 设置节点在K8S集群处于维护状态
	MaintainingInK8S
	// 节点处于SLURM不可用状态，SLURM服务失去响应
	SlurmUnHealth
	// 节点处于K8S不可用状态，K8S服务失去响应
	K8SUnHealth
	// 调度器设置节点归属于SLURM
	ScheduledForSlurm
	// 人工设置节点为SLURM所保留，此时若节点归属于SLURM则直接修改状态
	PreservedForSlurm
	// 人工设置节点为SLURM所保留，此时节点若归属于K8S则需要等待资源回收
	PreservingForSlurm
	// 调度器设置节点归属于K8S
	ScheduledForK8S
	// 人工设置节点为K8S所保留，此时若节点归属于K8S则直接修改状态
	PreservedForK8S
	// 人工设置节点为K8S所保留，此时节点若归属于SLURM则需要等待资源回收
	PreservingForK8S
	// 节点处于资源回收状态，该状态由调度器所触发
	ReclaimResource
	// 节点处于可分配状态
	NodeAllocatable
	// 节点状态占位符
	NodeStateEnd
)

// 用于将状态码映射成字符串
var StateToReason = map[int]string{
	MaintainingInSlurm: "MaintainingInSlurm",
	MaintainingInK8S:   "MaintainingInK8S",
	SlurmUnHealth:      "SlurmUnHealth",
	K8SUnHealth:        "K8SUnHealth",
	ScheduledForSlurm:  "ScheduledForSlurm",
	PreservedForSlurm:  "PreservedForSlurm",
	PreservingForSlurm: "PreservingForSlurm",
	ScheduledForK8S:    "ScheduledForK8S",
	PreservedForK8S:    "PreservedForK8S",
	PreservingForK8S:   "PreservingForK8S",
	ReclaimResource:    "ReclaimResource",
	NodeAllocatable:    "NodeAllocatable",
}

// 用于将字符串映射回状态码
var ReasonToState = map[string]int{
	"MaintainingInSlurm": MaintainingInSlurm,
	"MaintainingInK8S":   MaintainingInK8S,
	"SlurmUnHealth":      SlurmUnHealth,
	"K8SUnHealth":        K8SUnHealth,
	"ScheduledForSlurm":  ScheduledForSlurm,
	"PreservedForSlurm":  PreservedForSlurm,
	"PreservingForSlurm": PreservingForSlurm,
	"ScheduledForK8S":    ScheduledForK8S,
	"PreservedForK8S":    PreservedForK8S,
	"PreservingForK8S":   PreservingForK8S,
	"ReclaimResource":    ReclaimResource,
	"NodeAllocatable":    NodeAllocatable,
}

// 控制节点状态的接口
type NodeControl interface {
	DisableNode(node *Node) error
	EnableNode(node *Node) error
}
