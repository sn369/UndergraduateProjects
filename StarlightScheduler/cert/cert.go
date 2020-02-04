package main

import (
	"crypto/rand"
	"crypto/rsa"
	"crypto/x509"
	"io/ioutil"
	"log"
)

func main() {
	key, err := rsa.GenerateKey(rand.Reader, 2048)
	if err != nil {
		log.Fatalln(err)
	}
	err = ioutil.WriteFile("cert/scheduler.key", x509.MarshalPKCS1PrivateKey(key), 0400)
	if err != nil {
		log.Fatalln(err)
	}
}
