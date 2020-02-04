package main

import (
	"log"
	"starlight-scheduler/core"
	"starlight-scheduler/slurm"
)

func main() {
	client := slurm.NewSLURM()
	nodes := make(map[string]*core.Node)
	err := client.Parse(&nodes, nil)
	if err != nil {
		log.Fatalln(err)
	}
	for nodeName := range nodes {
		err = client.DisableNode(nodeName, "test")
		if err != nil {
			log.Fatalln(err)
		}
	}
	for nodeName := range nodes {
		err = client.EnableNode(nodeName, "")
		if err != nil {
			log.Fatalln(err)
		}
	}
}
