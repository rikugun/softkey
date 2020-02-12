package softkey

/*
#cgo CFLAGS: -I${SRCDIR}/lib/softkey
#cgo LDFLAGS: -L${SRCDIR}/lib/softkey -lsoftkey
#include "LoadConf.h"
*/
import "C"
import (
	"fmt"
	"strconv"
)

func GetChannelCount() int {
	if !KeyExists(){
		fmt.Printf("获取通道数配置失败")
		return 0
	}
	channelCount := C.GoString(C.loadChannelCount())
	// channelCount := "1"
	if count, err := strconv.Atoi(channelCount); err == nil {
		return count
	} else {
		fmt.Printf("get channel count err %v", err)
		return 0
	}
}

func KeyExists() bool {
	return C.keyExists() == 0
}

func main(){
    fmt.Printf("%d",GetChannelCount())
}