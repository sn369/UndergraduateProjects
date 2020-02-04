package core

import "github.com/dgrijalva/jwt-go"

type Token struct {
	Name string
	jwt.StandardClaims
}

const TokenName = "starlight-scheduler"
