package main

import (
	"bytes"
	"crypto/rsa"
	"crypto/x509"
	"encoding/json"
	"flag"
	"github.com/dgrijalva/jwt-go"
	"io/ioutil"
	"log"
	"net/http"
	"starlight-scheduler/core"
	"time"
)

var (
	client *http.Client
	token  string
	addr   *string
)

type Response struct {
	Spec string
	Err  string
}

func sendRequest(nodeNames, cluster, action string) *Response {
	var response Response
	data, err := json.Marshal(&struct {
		NodeNames string
		Cluster   string
	}{NodeNames: nodeNames, Cluster: cluster})
	if err != nil {
		response.Err = err.Error()
		return &response
	}
	req, err := http.NewRequest(http.MethodPatch, *addr+action, bytes.NewReader(data))
	if err != nil {
		response.Err = err.Error()
		return &response
	}
	req.Header.Set(core.TokenName, token)
	resp, err := client.Do(req)
	if err != nil {
		response.Err = err.Error()
		return &response
	}
	err = json.NewDecoder(resp.Body).Decode(&response)
	if err != nil {
		response.Err = err.Error()
		return &response
	}
	return &response
}

func preserveNodes(nodeNames, cluster string) *Response {
	return sendRequest(nodeNames, cluster, "/node/preserve")
}

func unPreserveNodes(nodeNames, cluster string) *Response {
	return sendRequest(nodeNames, cluster, "/node/unPreserve")
}

func maintainNodes(nodeNames, cluster string) *Response {
	return sendRequest(nodeNames, cluster, "/node/maintaining")
}

func unMaintainNodes(nodeNames, cluster string) *Response {
	return sendRequest(nodeNames, cluster, "/node/unMaintaining")
}

func getNodeState(nodeNames string) *Response {
	return sendRequest(nodeNames, "", "/node/state")
}

func main() {
	var (
		key      *rsa.PrivateKey
		response *Response
	)

	addr = flag.String("d", "http://localhost:5678", "融合调度的服务地址，默认http://localhost:5678")
	action := flag.String("a", "state", "要执行的动作(查询结点state，保留节点preserve，取消保留unpreserve，维护节点maintain，取消维护unmaintain)，默认state")
	cluster := flag.String("c", "slurm", "指定集群，默认slurm")
	nodes := flag.String("n", "", "要操作的节点列表，格式为XXX[YYY,ZZZ,SSS-EEE]，默认为空")
	secureKey := flag.String("s", "cert/scheduler.key", "安全认证文件，默认cert/scheduler.key")
	// 调用参数解析
	flag.Parse()
	if *nodes == "" && *action != "state"{
		flag.PrintDefaults()
		return
	}
	data, err := ioutil.ReadFile(*secureKey)
	if err != nil {
		log.Fatalln(err)
	}
	key, err = x509.ParsePKCS1PrivateKey(data)
	if err != nil {
		log.Fatalln(err)
	}
	claim := jwt.New(jwt.GetSigningMethod("RS256"))
	claim.Claims = core.Token{Name: core.TokenName}
	token, err = claim.SignedString(key)
	if err != nil {
		log.Fatalln(err)
	}
	client = &http.Client{Timeout: time.Duration(5) * time.Second}
	switch *action {
	case "preserve":
		response = preserveNodes(*nodes, *cluster)
	case "unpreserve":
		response = unPreserveNodes(*nodes, *cluster)
	case "maintain":
		response = maintainNodes(*nodes, *cluster)
	case "unmaintain":
		response = unMaintainNodes(*nodes, *cluster)
	case "state":
		response = getNodeState(*nodes)
	default:
		log.Fatalln("不支持的操作类型：" + *action)
	}
	if response.Err != "" {
		log.Fatalln(response.Err)
	}
	log.Println(response.Spec)
}
