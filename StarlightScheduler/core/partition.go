package core

// 计算分区的数据结构
type Partition struct {
	Name             string           // 分区名称
	State            int              // 分区状态
	NodeList         map[string]*Node // 分区包含的节点列表
	IdleNodes        map[string]*Node // 分区当前空闲的节点，不包含可分配状态的节点
	AllocatableNodes map[string]*Node // 分区当前可分配的节点
	NeededNodes      int              // 根据排队作业的信息决定分区所需的空闲节点数
	JobCount         int              // 已检查的排队作业的数目
}
