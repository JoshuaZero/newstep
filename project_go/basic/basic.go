package main

/*================================================================
*   Copyright (C) 2020 Sangfor Ltd. All rights reserved.
*
*    FileName：basic.go
*      Author：joshua_zero@outlook.com
*  CreateTime：2020年09月17日
* Describtion：--
*
================================================================*/
import (
	"fmt"
	"math"
	"math/cmplx"
)

func variableZeroValue() {
	var a int
	var s string
	fmt.Printf("%d, %q\n", a, s)

}

func variableInitialValue() {
	var a, b int = 2, 5
	var s string = "abv"
	fmt.Println(a, b, s)
}

func variableTypeDefine() {
	var a, b, c, s = 2, 5, true, "def"
	fmt.Println(a, b, c, s)
}

func variableShorter() {
	a, b, c, s := 2, 5, true, "def"
	fmt.Println(a, b, c, s)
}

func euler() {
	c := 3 + 4i
	fmt.Println(cmplx.Abs(c))

	fmt.Println(
		cmplx.Pow(math.E, 1i*math.Pi) + 1)

	fmt.Println(
		cmplx.Exp(1i*math.Pi) + 1)

	fmt.Printf("%.3f\n",
		cmplx.Exp(1i*math.Pi)+1)
}

func main() {
	fmt.Println("hello world!")
	variableZeroValue()
	variableInitialValue()
	variableTypeDefine()
	variableShorter()
	euler()
}
