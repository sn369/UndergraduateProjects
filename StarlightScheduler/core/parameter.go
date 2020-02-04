package core

var (
	// 是否从全局的角度平衡节点比例，值为false的话则按固定比例对分区进行调整
	BalanceGlobal bool
	// 可分配节点在空闲节点中的比例，百分比
	AllocatableNodeRatio int
	// SLURM空闲节点的比例，百分比
	SlurmIdleNodeRatio int
	// K8S空闲节点的比例，百分比
	K8SIdleNodeRatio int
	// 调度器的执行周期，单位秒
	SchedulingPeriod int
	// SLURM分区影响调度策略的排队作业数
	SLURMNumberQueueingJobForConsider int
)
