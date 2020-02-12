#ifndef SOFTKEYPWD_H
#define SOFTKEYPWD_H
#include <stdbool.h>
#include "libusb/libusb.h"

#define BYTE unsigned char
#define DWORD unsigned int
#define BOOL bool

#define PERMS (mode_t)(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define FLAGS (O_CREAT)

#define PID 0X8762
#define VID 0X3689
#define PID_NEW 0X2020
#define VID_NEW 0X3689
#define PID_NEW_2 0X2020
#define VID_NEW_2 0X2020

    //获到锁的版本
    int NT_GetVersion(int *Version,char *InPath);
    //获取锁的扩展版本
    int NT_GetVersionEx(int *Version,char *InPath);

	//算法函数
	int sWrite_2Ex_New(  DWORD in_data,DWORD *out_data,char *Path);
	int sWriteEx_New(  DWORD in_data,DWORD *out_data,char *Path);
	int sWrite_2Ex(  DWORD in_data,DWORD *out_data,char *Path);
	int sWriteEx(  DWORD in_data,DWORD *out_data,char *Path);
	int sRead(  DWORD *out_data,char *Path);
	int sWrite(  DWORD in_data,char *Path);
	int sWrite_2(  DWORD in_data,char *Path);


    //写一个字节到加密锁中
    int YWrite(BYTE InData,short address,char * HKey,char *LKey,char *Path );
    //从加密锁中读取一个字节
    int YRead(BYTE *OutData,short address,char * HKey,char *LKey,char *Path );
    //写一个字节到加密锁中
    int YWriteEx(BYTE *InData,short address,short len,char *HKey,char *LKey,char *Path );
    //从加密锁中读取一批字节
    int YReadEx(BYTE *OutData,short address,short len,char *HKey,char *LKey,char *Path );

	//查找指定的加密锁（使得普通算法一）
	int FindPort_2(  int start,DWORD in_data,DWORD verf_data,char *OutPath);
	//查找指定的加密锁（使得普通算法二）
	int FindPort_3(  int start,DWORD in_data,DWORD verf_data,char *OutPath);
	//查找加密锁
	int FindPort(  int start,char *OutPath);

    //获取锁的ID
    int GetID(  DWORD *ID_1,DWORD *ID_2,char *InPath);

    //从加密锁中读字符串
    int YReadString(char *string ,short Address,int len,char * HKey,char *LKey,char *Path );
    //写字符串到加密锁中
    int YWriteString(char *InString,short Address,char * HKey,char *LKey,char *Path );
    //设置写密码
    int SetWritePassword(char * W_HKey,char *W_LKey,char * new_HKey,char *new_LKey,char *InPath);
    //设置读密码
    int SetReadPassword(char * W_HKey,char *W_LKey,char * new_HKey,char *new_LKey,char *InPath);

    //设置增强算法密钥一
    int SetCal_2(char *Key,char *InPath);
    //使用增强算法一对字符串进行加密
    int EncString(  char *InString,char *OutString,char *Path);
    //使用增强算法一对二进制数据进行加密
    int Cal(  BYTE *InBuf,BYTE *OutBuf,char *Path);

	//设置增强算法密钥二
	int SetCal_New(char *Key,char *InPath);
	//使用增强算法二对字符串进行加密
	int Cal_New(  BYTE *InBuf,BYTE *OutBuf,char *Path);
	//使用增强算法二对字符串进行加密
	int EncString_New(  char *InString,char *OutString,char *Path);


    //使用增强算法对字符串进行解密(使用软件)
    void StrDec(  char *InString,char *OutString,char *Key);
    void StrEnc(  char *InString,char *OutString,char *Key);
    void EnCode(   BYTE  * InData,BYTE  * OutData,  char *Key  );
    void DeCode(   BYTE  * InData, BYTE  * OutData, char *Key  );
    //使用增强算法对二进制数据进行加密(使用软件)
    void  DecBySoft(    BYTE  *   aData,   BYTE   *   aKey   )  ;
    void  EncBySoft(   BYTE  *   aData,  BYTE   *   aKey   )   ;

    //字符串及二进制数据的转换
     void HexStringToByteArray(char * InString,BYTE *out_data);
     void ByteArrayToHexString(BYTE *in_data,char * OutString,int len);

     //初始化锁函数,注意，初始化锁后，所有的数据为0，读写密码也为00000000-00000000，增强算法不会被初始化
    int ReSet(char *InPath);

    //以下函数只限于带U盘的锁
    //设置是否显示U盘部分盘符，真为显示，否为不显示
    int SetHidOnly(  BOOL IsHidOnly,char *InPath);
    //设置U盘部分为只读状态，
    int SetUReadOnly( char *InPath);

    //以下函数只支持智能芯片的锁
    //生成SM2密钥对
    int YT_GenKeyPair(char* PriKey,char *PubKeyX,char *PubKeyY,char *InPath);
    //设置Pin码
    int YtSetPin(char *old_pin,char *new_pin,char *InPath );
    //设置SM2密钥对及身份
    int Set_SM2_KeyPair(char *PriKey,char *PubKeyX,char *PubKeyY,char *sm2UserName,char *InPath );
    //返回加密锁的公钥
    int Get_SM2_PubKey(char *PubKeyX,char *PubKeyY,char *sm2UserName,char *InPath);
    //对二进制数据进行SM2加密
    int SM2_EncBuf(BYTE *InBuf,BYTE *OutBuf,int inlen,char *InPath);
    //对二进制数据进行SM2解密
    int SM2_DecBuf(BYTE *InBuf,BYTE *OutBuf,int inlen,char* pin,char *InPath);
    //对字符串进行SM2加密
    int SM2_EncString(char *InString,char *OutString,char *InPath);
    //对字符串进行SM2解密
    int SM2_DecString(char *InString,char *OutString,char* pin,char *InPath);
    //返回锁的硬件芯片唯一ID
    int GetChipID( char *OutChipID,char *InPath);

	int GetChipID( char *OutChipID,char *DevicePath);
	//返回锁的出厂编码
	int GetProduceDate(  char *OutDate,char *DevicePath);
	//设置ID种子
	int SetID(char * Seed,char *DevicePath);
	//设置普通算法
	int SetCal(char * W_HKey,char *W_LKey,char * new_HKey,char *new_LKey,char *DevicePath);
	void SnToProduceDate(char* InSn,char *OutProduceDate);
	void CloseUsbHandle(char *DevicePath );

	int Hanldetransfe(libusb_device_handle *DevHandle,BYTE *array_in,int InLen,BYTE *array_out,int OutLen,int Report_size);
	libusb_device_handle * MyOpenPath(char *InPath,int *ret);
	int NT_GetVersion(int *Version,char *InPath);
	int NT_Read(  BYTE * ele1,BYTE * ele2,BYTE * ele3,BYTE * ele4,char *Path);
	int NT_Write(  BYTE * ele1,BYTE * ele2,BYTE * ele3,BYTE * ele4,char *Path);
	int ReadDword(  DWORD *in_data,char *Path);
	int WriteDword(  DWORD *out_data,char *Path);
	int Y_Write(BYTE *InData,short address,short len,BYTE *password,char *Path );
	int Y_Read(BYTE *OutData,short address ,short len,BYTE *password,char *Path );
	int NT_FindPort_2(  int start,DWORD in_data,DWORD verf_data,char *OutPath);
	int NT_FindPort(  int start,char *OutPath);
	int NT_GetID(  DWORD *ID_1,DWORD *ID_2,char *InPath);
	int NT_Write_2(  BYTE * ele1,BYTE * ele2,BYTE * ele3,BYTE * ele4,char *Path);
	int WriteDword_2(  DWORD *out_data,char *Path);
	int NT_SetCal_2(BYTE *InData,BYTE IsHi,char *Path );
	int NT_Cal(  BYTE * InBuf,BYTE *OutBuf,char *InPath);
	int NT_ReSet(char *Path );
	int isfindmydevice( int pos ,char *DevicePath);
	int  F_GetVerEx(int *Version,char *InPath);
	int NT_Write_New(  BYTE * ele1,BYTE * ele2,BYTE * ele3,BYTE * ele4,char *InPath);
	int NT_Write_2_New(  BYTE * ele1,BYTE * ele2,BYTE * ele3,BYTE * ele4,char *InPath);
	int NT_Cal_New(  BYTE * InBuf,BYTE *OutBuf,char *InPath);
	int NT_SetCal_New(BYTE *InData,BYTE IsHi,char *Path );
	int WriteDword_New(  DWORD *in_data,char *Path);
	int WriteDword_2_New(  DWORD *in_data,char *Path);
	int NT_FindPort_3(  int start,DWORD in_data,DWORD verf_data,char *OutPath);
	int NT_Set_SM2_KeyPair(BYTE *PriKey,BYTE *PubKeyX,BYTE *PubKeyY,char *sm2UserName,char *Path );
	int NT_Get_SM2_PubKey(BYTE *KGx,BYTE *KGy,char *sm2UserName,char *Path );
	int NT_GenKeyPair(BYTE* PriKey,BYTE *PubKey,char *Path );
	int NT_Set_Pin(char *old_pin,char *new_pin,char *Path );
	int NT_SM2_Enc(BYTE *inbuf,BYTE *outbuf,BYTE inlen,char *Path );
	int NT_SM2_Dec(BYTE *inbuf,BYTE *outbuf,BYTE inlen,char* pin,char *Path );
	int NT_Sign(BYTE *inbuf,BYTE *outbuf,char* pin,char *Path );
	int NT_Sign_2(BYTE *inbuf,BYTE *outbuf,char* pin,char *Path );
	int NT_Verfiy(BYTE *inbuf,BYTE *InSignBuf,BOOL *outbiao,char *Path );
	int  NT_GetChipID(  BYTE *OutChipID,char *InPath);
	int Sub_SetOnly(BOOL IsOnly,BYTE Flag,char *InPath);
	int NT_SetHidOnly(  BOOL IsHidOnly,char *InPath);
	int  NT_SetUReadOnly(char *InPath);
	int Read(BYTE *OutData,short address,BYTE *password,char *Path );
	int Write(BYTE InData,short address,BYTE *password,char *Path );
	int GetTrashBufLen(char * Path,int *OutLen);
	void myconvert(char *hkey,char *lkey,BYTE *out_data);
	DWORD HexToInt(char* s);
	void HexStringToByteArrayEx(char * InString,BYTE *in_data);
	int GetLen(char *InString);
	void SwitchByte2Char(char *outstring,BYTE *inbyte,int inlen);
	void SwitchChar2Byte(char *instring,BYTE *outbyte);
	void SwitchBigInteger2Byte(char *instring,BYTE *outbyte,int *outlen);
	int sSetHidOnly(BOOL IsHidOnly,char *KeyPath);


#endif // SOFTKEYYT88_H
