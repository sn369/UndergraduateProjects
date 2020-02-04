package main

import (
	"context"
	"flag"
	"log"
	"net/http"
	"os"
	"os/signal"
	"starlight-scheduler/core"
	schedulerV1 "starlight-scheduler/scheduler"
	"syscall"
)

// 星光调度器的入口函数
func main() {
	// 设置命令行需要解析的参数
	balanceGlobal := flag.Bool("b", false, "是否从全局的角度平衡节点比例，默认false，即按固定比例对分区进行调整")
	allocatableNodeRatio := flag.Int("a", 10, "可分配节点在空闲节点中的比例，百分比（0~99），默认10")
	k8sIdleNodeRatio := flag.Int("k", 25, "K8S空闲节点的比例，百分比（0~99），默认25")
	schedulingPeriod := flag.Int("p", 60, "调度器的执行周期，单位秒（1~900），默认60")
	sLURMNumberQueueingJobForConsider := flag.Int("j", 10, "SLURM分区影响调度策略的排队作业数（0~10000），默认10")
	k8sConfigFile := flag.String("c", "kube.config", "K8Sj集群的认证文件，默认kube.config")
	secureKey := flag.String("s", "cert/scheduler.key", "安全认证文件，默认cert/scheduler.key")
	// 调用参数解析
	flag.Parse()
	// 检查参数合法性
	if *allocatableNodeRatio < 0 || *allocatableNodeRatio > 99 || *k8sIdleNodeRatio < 0 || *k8sIdleNodeRatio > 99 || *schedulingPeriod < 1 || *schedulingPeriod > 900 ||
		*sLURMNumberQueueingJobForConsider < 0 || *sLURMNumberQueueingJobForConsider > 10000 {
		flag.PrintDefaults()
	}
	// 对调度的核心参数进行赋值
	core.BalanceGlobal, core.AllocatableNodeRatio, core.SlurmIdleNodeRatio, core.K8SIdleNodeRatio, core.SchedulingPeriod, core.SLURMNumberQueueingJobForConsider = *balanceGlobal, *allocatableNodeRatio, 100-*k8sIdleNodeRatio, *k8sIdleNodeRatio, *schedulingPeriod, *sLURMNumberQueueingJobForConsider
	// 创建调度器实例
	scheduler, err := schedulerV1.NewScheduler(*k8sConfigFile, *secureKey)
	if err != nil {
		log.Fatalln("无法创建调度器，失败原因：", err)
	}
	// 另起线程进行调度的处理
	go scheduler.Start()
	server := &http.Server{
		Addr:    "0.0.0.0:5678",
		Handler: scheduler.Router(),
	}
	errChan := make(chan error, 1)
	// 另起线程提供http服务
	go func() {
		errChan <- server.ListenAndServe()
	}()
	// 捕获系统信号
	signalChan := make(chan os.Signal, 1)
	signal.Notify(signalChan, syscall.SIGINT, syscall.SIGTERM)
	// 等待退出信号
	for {
		select {
		case s := <-signalChan:
			log.Printf("捕获到信号%v，准备停止服务\n", s)
			// 停止调度器
			scheduler.Stop()
			// 停止http服务
			server.Shutdown(context.Background())
			<-scheduler.StopChan
			log.Println("调度器停止完毕")
			<-errChan
			log.Println("http服务停止完毕")
			log.Println("服务全部停止完毕")
			return
		}
	}
}
