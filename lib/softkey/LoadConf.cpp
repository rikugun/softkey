#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "LoadConf.h"
#include "SoftkeyPWD.h"

char outstring[400] ;

char* loadChannelCount(){
    char KeyPath[260];
//    int ver;int verEx;
    int ret=FindPort(0,KeyPath);
    if(ret==0)
    {
        short mylen ;
        mylen = 6;//注意这里的6是长度，长度要与写入的字符串的长度相同
        memset(outstring,0,mylen+1);//outstring[mylen]=0;//将最后一个字符设置为0，即结束字符串
        if (YReadString(outstring, 0, mylen, "ffffffff", "ffffffff", KeyPath) != 0)
        {
            printf("读取通道配置失败\n");
            return "0";
        }
        else
        {
            printf("读取通道配置成功：%s\n",outstring);
            return outstring;
        }
    }
    return "";
}

int keyExists(){
    char KeyPath[260];
    return FindPort(0,KeyPath);
}
