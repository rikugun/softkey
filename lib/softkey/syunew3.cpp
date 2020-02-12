// jsyunew3.cpp : Defines the entry point for the DLL application.
//
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include "libusb/libusb.h"
#include <fcntl.h>
#include<string.h>
#include <dlfcn.h>
#include "UKey.h"
#include "KeyDef.h"
#include "SoftkeyPWD.h"

char sPubKeyX[ECC_MAXLEN*2+1];
char sPubKeyY[ECC_MAXLEN*2+1];
char sPriKey[ECC_MAXLEN*2+1];


int Hanldetransfe(libusb_device_handle *DevHandle,BYTE *array_in,int InLen,BYTE *array_out,int OutLen,int Report_size)
{
	int ret;
	if(InLen>0)
	{
		ret=libusb_control_transfer(DevHandle,0x21,9,0x0302,0,(unsigned char *)array_in,Report_size,0);
		if(ret<1)
		{
			libusb_close(DevHandle);return ErrSendData;
		}
	}
	if(OutLen>0)
	{
		ret=libusb_control_transfer(DevHandle,0xa1,1,0x0301,0,(unsigned char *)array_out,Report_size,0);
		if(ret<1)
		{
			libusb_close(DevHandle);return ErrGetData;
		}
	}

	return 0;
}

libusb_device_handle * MyOpenPath(char *InPath,int *ret)
{
	char *p;
	if(strlen(InPath)<1)
	{
		char OutPath[260];
		*ret=isfindmydevice(0,OutPath);
		if(*ret!=0){return NULL;}
		p=strchr(OutPath,':');

	}
	else
	{
		p=strchr(InPath,':');
	}
	if(p==NULL){*ret=NotFoundKey;return NULL;}
	p=p+1;
	char *stop;/*2017-1-4*/
	long long i_KeyPath=strtoll(p,&stop,10);/*2017-1-4*/
	libusb_device_handle * DevHandle =(libusb_device_handle *)i_KeyPath;
	return DevHandle;
}


int GetVersion(int *Version,char *DevicePath)
{
	int ret;
	BYTE array_in[500];BYTE array_out[500];

	libusb_device_handle * hUsbDevice=MyOpenPath(DevicePath,&ret);
	if(hUsbDevice == NULL)return ret;

	array_in[1]=GETVERSION;
	ret= Hanldetransfe(hUsbDevice,array_in,1,array_out,1,ReportLen_Usua);
	if(ret!=0)return ret;
	*Version=array_out[0];
	return 0x0;
}
int NT_FindPort(  int start,char *OutPath)
{
	return isfindmydevice(start,OutPath);
}

int Read(BYTE *OutData,short address,BYTE *password,char *DevicePath )
{
	BYTE opcode=0x80;
	int ret;
	BYTE array_in[500],array_out[500];int n;

	libusb_device_handle * hUsbDevice=MyOpenPath(DevicePath,&ret);
	if(hUsbDevice == NULL)return ret;

	if(address>495 || address<0)return ErrAddrOver;
	if(address>255){opcode=0xa0;address=address-256;}

	//array_in[1]Ҫ��������ݳ��ȣ�array_in[2]Ҫ���ܵ����ݳ��ȡ�array_in[3]ָ�
	//array_in[n+3]����
	array_in[1]=READBYTE;//read 0x10;write 0x11;
	array_in[2]=opcode;//0x01;//read 0x80 ;write 0x40;
	array_in[3]=(BYTE)address;
	for(n=5;n<14;n++)
	{
		array_in[n]=*password;
		password++;
	}
	ret= Hanldetransfe(hUsbDevice,array_in,13,array_out,2,ReportLen_Usua);
	if(ret!=0)return ret;
	if(array_out[0]!=0x53 )
	{
		return ErrReadPWD;//��ʾʧ�ܣ�
	}
	*OutData=array_out[1];
	return 0;
}

int  Write(BYTE InData,short address,BYTE *password,char *DevicePath )
{
	BYTE opcode=0x40;int ret;

	BYTE array_in[500],array_out[500];int n;

	libusb_device_handle * hUsbDevice=MyOpenPath(DevicePath,&ret);
	if(hUsbDevice == NULL)return ret;

	if(address>511 || address<0)return ErrAddrOver;
	if(address>255){opcode=0x60;address=address-256;}

	array_in[1]=WRITEBYTE;//read 0x10;write 0x11;
	array_in[2]=opcode;//0x01;//read 0x80 ;write 0x40;
	array_in[3]=(BYTE)address;
	array_in[4]=InData;
	for(n=5;n<14;n++)
	{
		array_in[n]=*password;
		password++;
	}
	ret= Hanldetransfe(hUsbDevice,array_in,13,array_out,2,ReportLen_Usua);
	if(ret!=0)return ret;
	if(array_out[1]!=1 )
	{
		return ErrWritePWD;//��ʾʧ�ܣ�
	}
	return 0;
}


int Y_Read(BYTE *OutData,short address ,short len,BYTE *password,char *DevicePath )
{
	short addr_l,addr_h;int n;int ret;

	BYTE array_in[512],array_out[512];

	libusb_device_handle * hUsbDevice=MyOpenPath(DevicePath,&ret);
	if(hUsbDevice == NULL)return ret;

	if(address>495 || address<0)return ErrAddrOver;
	if( len>255)return -87;
	if(len+address>511)return ErrAddrOver;
	{addr_h=(address>>8)<<1;addr_l=address&0xff;}

	//array_in[1]Ҫ��������ݳ��ȣ�array_in[2]Ҫ���ܵ����ݳ��ȡ�array_in[3]ָ�
	//array_in[n+3]����
	array_in[1]=YTREADBUF;//read 0x10;write 0x11;
	array_in[2]=(BYTE)addr_h;//0x01;//read 0x80 ;write 0x40;
	array_in[3]=(BYTE)addr_l;
	array_in[4]=(BYTE)len;
	for(n=0;n<8;n++)
	{
		array_in[5+n]=*password;
		password++;
	}
	ret= Hanldetransfe(hUsbDevice,array_in,13,array_out,len+1,ReportLen_Usua);
	if(ret!=0)return ret;

	if(array_out[0]!=0x0 )
	{
		return ErrReadPWD;//��ʾʧ�ܣ�
	}
	for(n=0;n<len;n++)
	{
		*OutData=array_out[n+1];
		OutData++;
	}
	return 0;
}

int Y_Write(BYTE *InData,short address,short len,BYTE *password,char *DevicePath )
{
	short addr_l,addr_h;int n;int ret;
	BYTE array_in[512],array_out[512];

	libusb_device_handle * hUsbDevice=MyOpenPath(DevicePath,&ret);
	if(hUsbDevice == NULL)return ret;

	if(len>255)return -87;
	if(address+len-1>511 || address<0)return ErrAddrOver;
	{addr_h=(address>>8)<<1;addr_l=address&0xff;}

	array_in[1]=YTWRITEBUF;//read 0x10;write 0x11;
	array_in[2]=(BYTE)addr_h;//0x01;//read 0x80 ;write 0x40;
	array_in[3]=(BYTE)addr_l;
	array_in[4]=(BYTE )len;
	for(n=0;n<8;n++)
	{
		array_in[5+n]=*password;
		password++;
	}
	for(n=0;n<len;n++)
	{
		array_in[13+n]=*InData;
		InData++;
	}
	ret= Hanldetransfe(hUsbDevice,array_in,13+len,array_out,3,ReportLen_Usua);
	if(ret!=0)return ret;

	if(array_out[0]!=0x0)
	{
		return ErrWritePWD;//��ʾʧ�ܣ�
	}
	return 0;
}

int isfindmydevice( int pos ,char *DevicePath)
{
	int count=0;
	int r;size_t i = 0;
	ssize_t cnt;
	libusb_device **devs;
	struct libusb_device *dev;
	libusb_device_handle *DevHandle;

	r = libusb_init(NULL);
	if (r < 0)
		return ErrlibusbInit;

	cnt = libusb_get_device_list(NULL, &devs);
	if (cnt < 0)
	{
		libusb_free_device_list(devs, 1);return ErrGetDevList;
	}

	while ((dev = devs[i++]) != NULL)
	{
		struct libusb_device_descriptor desc;
		r = libusb_get_device_descriptor(dev, &desc);
		if (r < 0) {
			libusb_free_device_list(devs, 1);return ErrGetDevDesc;
		}

		if(((desc.idProduct==PID) && (desc.idVendor==VID)) ||
			((desc.idProduct==PID_NEW) && (desc.idVendor==VID_NEW)) ||
			((desc.idProduct==PID_NEW_2) && (desc.idVendor==VID_NEW_2)))
		{
			if(count==pos)
			{
				int r=libusb_open(dev,&DevHandle);
				if(r<0)
				{
					return ErrlibusbOpen;
				}
				r=libusb_kernel_driver_active(DevHandle,0);
				if(r==1)r=libusb_detach_kernel_driver(DevHandle,0);
				r = libusb_claim_interface(DevHandle, 0);
				if (r < 0) {
					return ErrlibusbClaimInterface;
				}

				char temp_Path[260];
				long long temp=(long long)DevHandle;/*2017-1-4*/
				sprintf(temp_Path,"/dev/bus/usb/%03d/%03d %04x-%04x:%lld",/*2017-1-4*/
					libusb_get_bus_number(dev), libusb_get_device_address(dev),desc.idVendor, desc.idProduct,temp);
				strcpy(DevicePath,temp_Path);

				libusb_free_device_list(devs, 1);

				return 0;
			}
			count++;
		}

	}

	libusb_free_device_list(devs, 1);
	return NotFoundKey;
}

int NT_GetID(  DWORD *ID_1,DWORD *ID_2,char *DevicePath)
{
	DWORD t[8];int ret;
	BYTE array_in[500];BYTE array_out[500];

	libusb_device_handle * hUsbDevice=MyOpenPath(DevicePath,&ret);
	if(hUsbDevice == NULL)return ret;

	array_in[1]=GETID;
	ret= Hanldetransfe(hUsbDevice,array_in,1,array_out,8,ReportLen_Usua);
	if(ret!=0)return ret;
	t[0]=array_out[0];t[1]=array_out[1];t[2]=array_out[2];t[3]=array_out[3];
	t[4]=array_out[4];t[5]=array_out[5];t[6]=array_out[6];t[7]=array_out[7];
	*ID_1=t[3]|(t[2]<<8)|(t[1]<<16)|(t[0]<<24);
	*ID_2=t[7]|(t[6]<<8)|(t[5]<<16)|(t[4]<<24);
	return 0;
}

int NT_Cal(  BYTE * InBuf,BYTE *OutBuf,char *DevicePath)
{
	int n;int ret;
	BYTE array_in[500];BYTE array_out[500];

	libusb_device_handle * hUsbDevice=MyOpenPath(DevicePath,&ret);
	if(hUsbDevice == NULL)return ret;

	array_in[1]=CAL_TEA;
	for (n=2;n<10;n++)
	{
		array_in[n]=InBuf[n-2];
	}
	ret= Hanldetransfe(hUsbDevice,array_in,9,array_out,10,ReportLen_Usua);
	if(ret!=0)return ret;
	memcpy(OutBuf,array_out,8);
	if(array_out[8]!=0x55)
	{
		return ErrResult;
	}
	return 0;
}

int NT_SetCal_2(BYTE *InData,BYTE IsHi,char *DevicePath )
{
	int n;int ret;
	BYTE array_in[30],array_out[500];
	BYTE opcode=SET_TEAKEY;

	libusb_device_handle * hUsbDevice=MyOpenPath(DevicePath,&ret);
	if(hUsbDevice == NULL)return ret;

	array_in[1]=opcode;
	array_in[2]=IsHi;
	for(n=0;n<8;n++)
	{
		array_in[3+n]=*InData;
		InData++;
	}
	ret= Hanldetransfe(hUsbDevice,array_in,11,array_out,3,ReportLen_Usua);
	if(ret!=0)return ret;
	if(array_out[0]!=0x0)
	{
		return ErrResult;//��ʾʧ�ܣ�
	}
	return 0;
}

int NT_ReSet(char *DevicePath )
{

	int ret;
	BYTE array_in[30],array_out[500];

	libusb_device_handle * hUsbDevice=MyOpenPath(DevicePath,&ret);
	if(hUsbDevice == NULL)return ret;

	array_in[1]=MYRESET;
	ret= Hanldetransfe(hUsbDevice,array_in,2,array_out,1,ReportLen_Usua);
	if(ret!=0)return ret;
	if(ret!=0)return ret;
	if(array_out[0]!=0x0)
	{
		return ErrResult;//��ʾʧ�ܣ�
	}
	return 0;
}

int  F_GetVerEx(int *Version,char *DevicePath)
{
	int ret;
	BYTE array_in[500];BYTE array_out[500];

	libusb_device_handle * hUsbDevice=MyOpenPath(DevicePath,&ret);
	if(hUsbDevice == NULL)return ret;

	array_in[1]=GETVEREX;
	ret= Hanldetransfe(hUsbDevice,array_in,1,array_out,1,ReportLen_Usua);
	if(ret!=0)return ret;
	*Version=array_out[0];
	return 0x0;
}


int NT_Cal_New(  BYTE * InBuf,BYTE *OutBuf,char *DevicePath)
{
	int n,ret;
	BYTE array_in[500];BYTE array_out[500];

	libusb_device_handle * hUsbDevice=MyOpenPath(DevicePath,&ret);
	if(hUsbDevice == NULL)return ret;

	array_in[1]=CAL_TEA_2;
	for (n=2;n<10;n++)
	{
		array_in[n]=InBuf[n-2];
	}
	ret= Hanldetransfe(hUsbDevice,array_in,9,array_out,10,ReportLen_Usua);
	if(ret!=0)return ret;
	memcpy(OutBuf,array_out,8);
	if(array_out[8]!=0x55)
	{
		return ErrResult;
	}

	return 0;
}

int NT_SetCal_New(BYTE *InData,BYTE IsHi,char *DevicePath )
{
	int n,ret;
	BYTE array_in[30],array_out[500];
	BYTE opcode=SET_TEAKEY_2;

	libusb_device_handle * hUsbDevice=MyOpenPath(DevicePath,&ret);
	if(hUsbDevice == NULL)return ret;

	array_in[1]=opcode;
	array_in[2]=IsHi;
	for(n=0;n<8;n++)
	{
		array_in[3+n]=*InData;
		InData++;
	}
	ret= Hanldetransfe(hUsbDevice,array_in,11,array_out,3,ReportLen_Usua);
	if(ret!=0)return ret;
	if(array_out[0]!=0x0)
	{
		return ErrResult;//��ʾʧ�ܣ�
	}
	return 0;
}

int NT_Set_SM2_KeyPair(BYTE *PriKey,BYTE *PubKeyX,BYTE *PubKeyY,char *sm2UserName,char *DevicePath )
{

	int ret;
	BYTE array_in[500],array_out[500];

	libusb_device_handle * hUsbDevice=MyOpenPath(DevicePath,&ret);
	if(hUsbDevice == NULL)return ret;

	memset(array_in,0,256);

	array_in[1]=SET_ECC_KEY;
	memcpy(&array_in[2+ECC_MAXLEN*0],PriKey,ECC_MAXLEN);
	memcpy(&array_in[2+ECC_MAXLEN*1],PubKeyX,ECC_MAXLEN);
	memcpy(&array_in[2+ECC_MAXLEN*2],PubKeyY,ECC_MAXLEN);
	if(sm2UserName!=0)
	{
		memcpy(&array_in[2+ECC_MAXLEN*3],sm2UserName,USERID_LEN);
	}
	ret= Hanldetransfe(hUsbDevice,array_in,ECC_MAXLEN*3+2+USERID_LEN,array_out,1,ReportLen_Smart);
	if(ret!=0)return ret;
	if(array_out[0]!=0x20)return USBStatusFail;

	return 0;
}

int NT_Get_SM2_PubKey(BYTE *KGx,BYTE *KGy,char *sm2UserName,char *DevicePath )
{

	int ret;
	BYTE array_in[500],array_out[500];

	array_out[0]=0xfb;

	libusb_device_handle * hUsbDevice=MyOpenPath(DevicePath,&ret);
	if(hUsbDevice == NULL)return ret;

	array_in[1]=GET_ECC_KEY;
	ret= Hanldetransfe(hUsbDevice,array_in,2,array_out,ECC_MAXLEN*2+2+USERID_LEN,ReportLen_Smart);
	if(ret!=0)return ret;

	if(array_out[0]!=0x20)return USBStatusFail;
	memcpy(KGx,&array_out[1+ECC_MAXLEN*0],ECC_MAXLEN);
	memcpy(KGy,&array_out[1+ECC_MAXLEN*1],ECC_MAXLEN);
	memcpy(sm2UserName,&array_out[1+ECC_MAXLEN*2],USERID_LEN);

	return 0;
}


int NT_GenKeyPair(BYTE* PriKey,BYTE *PubKey,char *DevicePath )
{

	int ret;
	BYTE array_in[500],array_out[500];

	array_out[0]=0xfb;

	libusb_device_handle * hUsbDevice=MyOpenPath(DevicePath,&ret);
	if(hUsbDevice == NULL)return ret;

	array_in[1]=GEN_KEYPAIR;
	ret= Hanldetransfe(hUsbDevice,array_in,2,array_out,ECC_MAXLEN*3+2,ReportLen_Smart);
	if(ret!=0)return ret;

	if(array_out[0]!=0x20)
	{
		return FAILEDGENKEYPAIR;//��ʾ��ȡʧ�ܣ�
	}
	memcpy(PriKey,&array_out[1+ECC_MAXLEN*0],ECC_MAXLEN);
	memcpy(PubKey,&array_out[1+ECC_MAXLEN*1],ECC_MAXLEN*2+1);
	return 0;
}

int NT_Set_Pin(char *old_pin,char *new_pin,char *DevicePath )
{

	int ret;
	BYTE array_in[500],array_out[500];

	libusb_device_handle * hUsbDevice=MyOpenPath(DevicePath,&ret);
	if(hUsbDevice == NULL)return ret;

	array_in[1]=SET_PIN;
	memcpy(&array_in[2+PIN_LEN*0],old_pin,PIN_LEN);
	memcpy(&array_in[2+PIN_LEN*1],new_pin,PIN_LEN);

	ret= Hanldetransfe(hUsbDevice,array_in,PIN_LEN*2+2,array_out,2,ReportLen_Smart);
	if(ret!=0)return ret;

	if(array_out[0]!=0x20)return USBStatusFail;
	if(array_out[1]!=0x20)return FAILPINPWD;
	return 0;
}


int NT_SM2_Enc(BYTE *inbuf,BYTE *outbuf,BYTE inlen,char *DevicePath )
{

	int n;int ret;
	BYTE array_in[500],array_out[500];

	libusb_device_handle * hUsbDevice=MyOpenPath(DevicePath,&ret);
	if(hUsbDevice == NULL)return ret;

	array_out[0]=0xfb;
	array_in[1]=MYENC;
	array_in[2]=inlen;
	for(n=0;n<inlen;n++)
	{
		array_in[3+n]=inbuf[n];
	}
	ret= Hanldetransfe(hUsbDevice,array_in,inlen+1+2,array_out,inlen+SM2_ADDBYTE+3,ReportLen_Smart);
	if(ret!=0)return ret;

	if(array_out[0]!=0x20)return USBStatusFail;
	if(array_out[1]==0)return FAILENC;

	memcpy(outbuf,&array_out[2],inlen+SM2_ADDBYTE);

	return 0;
}

int NT_SM2_Dec(BYTE *inbuf,BYTE *outbuf,BYTE inlen,char* pin,char *DevicePath )
{

	int n;int ret;
	BYTE array_in[500],array_out[500];

	libusb_device_handle * hUsbDevice=MyOpenPath(DevicePath,&ret);
	if(hUsbDevice == NULL)return ret;

	array_out[0]=0xfb;
	array_in[1]=MYDEC;
	memcpy(&array_in[2],pin,PIN_LEN);
	array_in[2+PIN_LEN]=inlen;
	for(n=0;n<inlen;n++)
	{
		array_in[2+PIN_LEN+1+n]=inbuf[n];
	}
	ret= Hanldetransfe(hUsbDevice,array_in,inlen+1+2+PIN_LEN,array_out,inlen-SM2_ADDBYTE+4,ReportLen_Smart);
	if(ret!=0)return ret;

	if(array_out[2]!=0x20)return FAILPINPWD;
	if(array_out[1]==0) return FAILENC;
	if(array_out[0]!=0x20)return USBStatusFail;
	memcpy(outbuf,&array_out[3],inlen-SM2_ADDBYTE);

	return 0;
}

int NT_Sign(BYTE *inbuf,BYTE *outbuf,char* pin,char *DevicePath )
{

	int n;int ret;
	BYTE array_in[500],array_out[500];

	libusb_device_handle * hUsbDevice=MyOpenPath(DevicePath,&ret);
	if(hUsbDevice == NULL)return ret;

	array_out[0]=0xfb;
	array_in[1]=YTSIGN;
	memcpy(&array_in[2],pin,PIN_LEN);
	for(n=0;n<32;n++)
	{
		array_in[2+PIN_LEN+n]=inbuf[n];
	}
	ret= Hanldetransfe(hUsbDevice,array_in,32+2+PIN_LEN,array_out,64+2,ReportLen_Smart);
	if(ret!=0)return ret;

	if(array_out[1]!=0x20)return FAILPINPWD;
	if(array_out[0]!=0x20)return USBStatusFail;
	memcpy(outbuf,&array_out[2],64);

	return 0;
}

int NT_Sign_2(BYTE *inbuf,BYTE *outbuf,char* pin,char *DevicePath )
{

	int n;int ret;
	BYTE array_in[500],array_out[500];

	libusb_device_handle * hUsbDevice=MyOpenPath(DevicePath,&ret);
	if(hUsbDevice == NULL)return ret;

	array_out[0]=0xfb;
	array_in[1]=YTSIGN_2;
	memcpy(&array_in[2],pin,PIN_LEN);
	for(n=0;n<32;n++)
	{
		array_in[2+PIN_LEN+n]=inbuf[n];
	}
	ret= Hanldetransfe(hUsbDevice,array_in,32+2+PIN_LEN,array_out,64+2,ReportLen_Smart);
	if(ret!=0)return ret;
	if(array_out[1]!=0x20)return FAILPINPWD;
	if(array_out[0]!=0x20)return USBStatusFail;
	memcpy(outbuf,&array_out[2],64);

	return 0;
}

int NT_Verfiy(BYTE *inbuf,BYTE *InSignBuf,BOOL *outbiao,char *DevicePath )
{

	int n;int ret;
	BYTE array_in[500],array_out[500];

	libusb_device_handle * hUsbDevice=MyOpenPath(DevicePath,&ret);
	if(hUsbDevice == NULL)return ret;

	array_out[0]=0xfb;
	array_in[1]=YTVERIFY;
	for(n=0;n<32;n++)
	{
		array_in[2+n]=inbuf[n];
	}
	for(n=0;n<64;n++)
	{
		array_in[2+32+n]=InSignBuf[n];
	}
	ret= Hanldetransfe(hUsbDevice,array_in,32+2+64,array_out,3,ReportLen_Smart);
	if(ret!=0)return ret;
	*outbiao=array_out[1];
	if(array_out[0]!=0x20)return USBStatusFail;

	return 0;
}

int  NT_GetChipID(  BYTE *OutChipID,char *DevicePath)
{
	int ret;
	BYTE array_in[500];BYTE array_out[500];

	libusb_device_handle * hUsbDevice=MyOpenPath(DevicePath,&ret);
	if(hUsbDevice == NULL)return ret;

	array_in[1]=GET_CHIPID;
	ret= Hanldetransfe(hUsbDevice,array_in,1,array_out,17,ReportLen_Smart);
	if(ret!=0)return ret;
	if(array_out[0]!=0x20)return USBStatusFail;
	memcpy(OutChipID,&array_out[1],16);

	return 0;
}

int Sub_SetOnly(BOOL IsOnly,BYTE Flag,char *DevicePath)
{
	int ret;
	BYTE array_in[500];BYTE array_out[500];

	libusb_device_handle * hUsbDevice=MyOpenPath(DevicePath,&ret);
	if(hUsbDevice == NULL)return ret;

	array_in[1]=Flag;
	if(IsOnly)array_in[2]=0;else array_in[2]=0xff;
	ret=Hanldetransfe(hUsbDevice,array_in,3,array_out,1,ReportLen_Smart);
	if(ret!=0)return ret;
	if(array_out[0]!=0x0)
	{
		return ErrResult;//��ʾʧ�ܣ�
	}
	return 0;
}

int NT_SetHidOnly(  BOOL IsHidOnly,char *DevicePath)
{
	return Sub_SetOnly(IsHidOnly,SETHIDONLY,DevicePath);
}

int  NT_SetUReadOnly(char *DevicePath)
{
	return Sub_SetOnly(true,SETREADONLY,DevicePath);
}


int NT_GetVersion(int *Version,char *DevicePath)
{
	int ret;
	sem_t * Sem =   sem_open("ex_sim",FLAGS , PERMS, 1);
	sem_wait(Sem);
	ret=(GetVersion)(Version,DevicePath);
	sem_post(Sem);
	sem_close(Sem);
	return ret;
}

int GetID(  DWORD *ID_1,DWORD *ID_2,char *DevicePath)
{
	int ret;
	sem_t * Sem =   sem_open("ex_sim",FLAGS , PERMS, 1);
	sem_wait(Sem);
	ret=(NT_GetID)(ID_1,ID_2,DevicePath);
	sem_post(Sem);
	sem_close(Sem);
	return ret;
}

int NT_Read(  BYTE * ele1,BYTE * ele2,BYTE * ele3,BYTE * ele4,char *DevicePath)
{
	int ret;
	BYTE array_out[500];

	libusb_device_handle * hUsbDevice=MyOpenPath(DevicePath,&ret);
	if(hUsbDevice == NULL)return ret;

	ret= Hanldetransfe(hUsbDevice,NULL,0,array_out,4,ReportLen_Usua);
	if(ret!=0)return ret;
	*ele1=array_out[0];
	*ele2=array_out[1];
	*ele3=array_out[2];
	*ele4=array_out[3];
	return 0;
}

int NT_Write(  BYTE * ele1,BYTE * ele2,BYTE * ele3,BYTE * ele4,char *DevicePath)
{
	int ret;
	BYTE array_in[500];

	libusb_device_handle * hUsbDevice=MyOpenPath(DevicePath,&ret);
	if(hUsbDevice == NULL)return ret;

	array_in[1]=3;array_in[2]=*ele1;array_in[3]=*ele2;array_in[4]=*ele3;array_in[5]=*ele4;//	array_in[1]=*ele1;array_in[2]=*ele2;
	ret= Hanldetransfe(hUsbDevice,array_in,5,NULL,0,ReportLen_Usua);
	return ret;
}



int NT_Write_2(  BYTE * ele1,BYTE * ele2,BYTE * ele3,BYTE * ele4,char *DevicePath)
{
	int ret;
	BYTE array_in[500];

	libusb_device_handle * hUsbDevice=MyOpenPath(DevicePath,&ret);
	if(hUsbDevice == NULL)return ret;

	array_in[1]=4;array_in[2]=*ele1;array_in[3]=*ele2;array_in[4]=*ele3;array_in[5]=*ele4;//	array_in[1]=*ele1;array_in[2]=*ele2;
	ret= Hanldetransfe(hUsbDevice,array_in,5,NULL,0,ReportLen_Usua);
	return ret;
}

int NT_Write_New(  BYTE * ele1,BYTE * ele2,BYTE * ele3,BYTE * ele4,char *DevicePath)
{
	int ret;
	BYTE array_in[500];
	libusb_device_handle * hUsbDevice=MyOpenPath(DevicePath,&ret);
	if(hUsbDevice == NULL)return ret;

	array_in[1]=0x0A;array_in[2]=*ele1;array_in[3]=*ele2;array_in[4]=*ele3;array_in[5]=*ele4;//	array_in[1]=*ele1;array_in[2]=*ele2;
	ret= Hanldetransfe(hUsbDevice,array_in,5,NULL,0,ReportLen_Usua);
	return ret;
}


int NT_Write_2_New(  BYTE * ele1,BYTE * ele2,BYTE * ele3,BYTE * ele4,char *DevicePath)
{
	int ret;
	BYTE array_in[500];
	libusb_device_handle * hUsbDevice=MyOpenPath(DevicePath,&ret);
	if(hUsbDevice == NULL)return ret;

	array_in[1]=0xB;array_in[2]=*ele1;array_in[3]=*ele2;array_in[4]=*ele3;array_in[5]=*ele4;//	array_in[1]=*ele1;array_in[2]=*ele2;
	ret= Hanldetransfe(hUsbDevice,array_in,5,NULL,0,ReportLen_Usua);
	return ret;
}


int ReadDword(  DWORD *in_data,char *DevicePath)
{
	BYTE b[4];int result;
	DWORD t[4];
	result= NT_Read(&b[0],&b[1],&b[2],&b[3],DevicePath);
	t[0]=b[0];t[1]=b[1];t[2]=b[2];t[3]=b[3];
	*in_data=t[0]|(t[1]<<8)|(t[2]<<16)|(t[3]<<24);
	return result;
}

int WriteDword(  DWORD *in_data,char *DevicePath)
{
	BYTE b[4];
	b[0]=(BYTE)*in_data;b[1]=(BYTE)(*in_data>>8);
	b[2]=(BYTE)(*in_data>>16);b[3]=(BYTE)(*in_data>>24);
	return NT_Write(&b[0],&b[1],&b[2],&b[3],DevicePath);
}

int WriteDword_2(  DWORD *in_data,char *DevicePath)
{
	BYTE b[4];
	b[0]=(BYTE)*in_data;b[1]=(BYTE)(*in_data>>8);
	b[2]=(BYTE)(*in_data>>16);b[3]=(BYTE)(*in_data>>24);
	return NT_Write_2(&b[0],&b[1],&b[2],&b[3],DevicePath);
}



int sRead(  DWORD *in_data,char *DevicePath)
{
	int ret;
	//////////////////////////////////////////////////////////////////////////////////

	sem_t * Sem =   sem_open("ex_sim",FLAGS , PERMS, 1);
	sem_wait(Sem);
	ret=(ReadDword)(in_data,DevicePath);
	sem_post(Sem);
	sem_close(Sem);
	return ret;
}

int sWrite(  DWORD out_data,char *DevicePath)
{
	int ret;
	sem_t * Sem =   sem_open("ex_sim",FLAGS , PERMS, 1);
	sem_wait(Sem);
	ret=(WriteDword)(&out_data,DevicePath);
	sem_post(Sem);
	sem_close(Sem);
	return ret;
}

int YWrite(BYTE InData,short address,char * HKey,char *LKey,char *DevicePath )
{
	if(address>495){return  -81;}
	BYTE ary1[8];
	myconvert(HKey,LKey,ary1);
	int ret;
	sem_t * Sem =   sem_open("ex_sim",FLAGS , PERMS, 1);
	sem_wait(Sem);
	ret=(Write)(InData,address,ary1,DevicePath);
	sem_post(Sem);
	sem_close(Sem);
	return ret;
}

int YRead(BYTE *OutData,short address,char * HKey,char *LKey,char *DevicePath )
{
	if(address>495){return  -81;}
	BYTE ary1[8];
	myconvert(HKey,LKey,ary1);
	int ret;
	sem_t * Sem =   sem_open("ex_sim",FLAGS , PERMS, 1);
	sem_wait(Sem);
	ret=(Read)(OutData,address,ary1,DevicePath);
	sem_post(Sem);
	sem_close(Sem);
	return ret;
}

int YWriteEx(BYTE *InData,short Address,short len,char *HKey,char *LKey,char *DevicePath )
{
	int ret;BYTE password[8];int n,trashLen=8;int temp_leave, leave;
	if(Address+len-1>495 || Address<0)return -81;

	myconvert(HKey,LKey,password);

	sem_t * Sem =   sem_open("ex_sim",FLAGS , PERMS, 1);

	sem_wait(Sem);
	temp_leave=Address-Address/trashLen*trashLen; leave=trashLen-temp_leave;
	if(leave>len){leave=len;}
	if(leave>0)
	{
		for(n=0;n<leave/trashLen;n++)
		{
			ret=(Y_Write)(InData+n*trashLen,Address+n*trashLen,trashLen,password,DevicePath);
			if(ret!=0){sem_post(Sem);sem_close(Sem);return  ret;}
		}
		if(leave-trashLen*n>0)
		{
			ret=(Y_Write)(InData+n*trashLen,Address+n*trashLen,(BYTE)(leave-n*trashLen),password,DevicePath);
			if(ret!=0){sem_post(Sem);sem_close(Sem);return  ret;}

		}
	}
	len=len-leave;Address=Address+leave;InData=InData+leave;
	if (len>0)
	{
		for(n=0;n<len/trashLen;n++)
		{
			ret=(Y_Write)(InData+n*trashLen,Address+n*trashLen,trashLen,password,DevicePath);
			if(ret!=0){sem_post(Sem);sem_close(Sem);return ret;}
		}
		if(len-trashLen*n>0)
		{
			ret=(Y_Write)(InData+n*trashLen,Address+n*trashLen,(BYTE)(len-n*trashLen),password,DevicePath);
			if(ret!=0){sem_post(Sem);sem_close(Sem);return ret;}

		}
	}
	sem_post(Sem);
	sem_close(Sem);
	return ret;
}

int YReadEx(BYTE *OutData,short Address,short len,char *HKey,char *LKey,char *DevicePath )
{
	int ret;BYTE password[8];int n,trashLen=16;
	if(Address+len-1>495 || Address<0)return -81;

	myconvert(HKey,LKey,password);

	sem_t * Sem =   sem_open("ex_sim",FLAGS , PERMS, 1);
	sem_wait(Sem);
	for(n=0;n<len/trashLen;n++)
	{
		ret=(Y_Read)(OutData+n*trashLen,Address+n*trashLen,trashLen,password,DevicePath);
		if(ret!=0){sem_post(Sem);sem_close(Sem);return ret;}
	}
	if(len-trashLen*n>0)
	{
		ret=(Y_Read)(OutData+n*trashLen,Address+n*trashLen,(len-trashLen*n),password,DevicePath);
		if(ret!=0){sem_post(Sem);sem_close(Sem);return ret;}

	}
	sem_post(Sem);
	sem_close(Sem);
	return ret;
}


int WriteDword_New(  DWORD *in_data,char *Path)
{
	BYTE b[4];
	b[0]=(BYTE)*in_data;b[1]=(BYTE)(*in_data>>8);
	b[2]=(BYTE)(*in_data>>16);b[3]=(BYTE)(*in_data>>24);
	return NT_Write_New(&b[0],&b[1],&b[2],&b[3],Path);
}

int WriteDword_2_New(  DWORD *in_data,char *Path)
{
	BYTE b[4];
	b[0]=(BYTE)*in_data;b[1]=(BYTE)(*in_data>>8);
	b[2]=(BYTE)(*in_data>>16);b[3]=(BYTE)(*in_data>>24);
	return NT_Write_2_New(&b[0],&b[1],&b[2],&b[3],Path);
}

int NT_FindPort_2(  int start,DWORD in_data,DWORD verf_data,char *OutPath)
{
	int pos;DWORD out_data;int ret;
	for(pos=start;pos<127;pos++)
	{
		ret=isfindmydevice(pos,OutPath);
		if(ret!=0){return ret;}
		ret=WriteDword( &in_data,OutPath);
		if(ret!=0){continue;}
		ret=ReadDword( &out_data,OutPath);
		if(ret!=0){continue;}
		if(out_data==verf_data){return 0;}
	}
	return NotFoundKey;
}

int NT_FindPort_3(  int start,DWORD in_data,DWORD verf_data,char *OutPath)
{
	int pos;DWORD out_data;int ret;
	for(pos=start;pos<127;pos++)
	{
		ret=isfindmydevice(pos,OutPath);
		if(ret!=0){return ret;}
		ret=WriteDword_New( &in_data,OutPath);
		if(ret!=0){continue;;}
		ret=ReadDword( &out_data,OutPath);
		if(ret!=0){continue;}
		if(out_data==verf_data){return 0;}
	}
	return NotFoundKey;
}


int sWrite_2(  DWORD out_data,char *DevicePath)
{
	int ret;
	sem_t * Sem =   sem_open("ex_sim",FLAGS , PERMS, 1);
	sem_wait(Sem);
	ret=(WriteDword_2)(&out_data,DevicePath);
	sem_post(Sem);
	sem_close(Sem);
	return ret;
}

int FindPort_3(  int start,DWORD in_data,DWORD verf_data,char *OutPath)
{
	int ret;
	sem_t * Sem =   sem_open("ex_sim",FLAGS , PERMS, 1);
	sem_wait(Sem);
	ret=(NT_FindPort_3)(start,in_data,verf_data,OutPath);
	sem_post(Sem);
	sem_close(Sem);
	return ret;
}

int FindPort_2(  int start,DWORD in_data,DWORD verf_data,char *OutPath)
{
	int ret;
	sem_t * Sem =   sem_open("ex_sim",FLAGS , PERMS, 1);
	sem_wait(Sem);
	ret=(NT_FindPort_2)(start,in_data,verf_data,OutPath);
	sem_post(Sem);
	sem_close(Sem);
	return ret;
}


int FindPort(  int start,char *OutPath)
{
	int ret;
	sem_t * Sem =   sem_open("ex_sim",FLAGS , PERMS, 1);
	sem_wait(Sem);
	ret=(NT_FindPort)(start,OutPath);
	sem_post(Sem);
	sem_close(Sem);
	return ret;
}


void myconvert(char *hkey,char *lkey,BYTE *out_data)
{
	DWORD z,z1;int n;
	z=HexToInt(hkey);
	z1=HexToInt(lkey);
	for(n=0;n<=3;n++)
	{
		*out_data=(BYTE)((z<<(n*8))>>24);
		out_data++;
	}
	for(n=0;n<=3;n++)
	{
		*out_data=(BYTE)((z1<<(n*8))>>24);
		out_data++;
	}
}

DWORD HexToInt(char* s)
{
	char hexch[] = "0123456789ABCDEF";
	size_t i;
	DWORD r,n,k,j;
	char ch;

	k=1; r=0;
	for (i=strlen(s);  i>0; i--) {
		ch = s[i-1]; if (ch > 0x3f) ch &= 0xDF;
		n = 0;
		for (j = 0; j<16; j++)
			if (ch == hexch[j])
				n = j;
		r += (n*k);
		k *= 16;
	}
	return r;
}

int SetReadPassword(char * W_HKey,char *W_LKey,char * new_HKey,char *new_LKey,char *DevicePath)
{

	BYTE ary1[8];BYTE ary2[8];
	myconvert(W_HKey,W_LKey,ary1);
	myconvert(new_HKey,new_LKey,ary2);
	int ret;int address=0x1f0;
	sem_t * Sem =   sem_open("ex_sim",FLAGS , PERMS, 1);
	sem_wait(Sem);
	ret=(Y_Write)(ary2,address,8,ary1,DevicePath);
	if(ret!=0){sem_post(Sem);return ret;}
	sem_post(Sem);
	sem_close(Sem);
	return ret;

}


int SetWritePassword(char * W_HKey,char *W_LKey,char * new_HKey,char *new_LKey,char *DevicePath)
{
	BYTE ary1[8];BYTE ary2[8];
	myconvert(W_HKey,W_LKey,ary1);
	myconvert(new_HKey,new_LKey,ary2);
	int ret;int address=0x1f8;
	sem_t * Sem =   sem_open("ex_sim",FLAGS , PERMS, 1);
	sem_wait(Sem);
	ret=(Y_Write)(ary2,address,8,ary1,DevicePath);
	if(ret!=0){sem_post(Sem);return ret;}
	sem_post(Sem);
	sem_close(Sem);
	return ret;

}

int YWriteString(char *InString,short Address,char * HKey,char *LKey,char *DevicePath )
{
	BYTE ary1[8];int n,trashLen=8;int ret=0,outlen,total_len;//int versionex;

	myconvert(HKey,LKey,ary1);

	outlen=strlen(InString);
	total_len=Address+outlen;
	if(total_len>495){return -47;}

	sem_t * Sem =   sem_open("ex_sim",FLAGS , PERMS, 1);
	{
		sem_wait(Sem);
		for(n=0;n<outlen/trashLen;n++)
		{
			ret=(Y_Write)((BYTE *)(InString+n*trashLen),Address+n*trashLen,trashLen,ary1,DevicePath);
			if(ret!=0){sem_post(Sem);sem_close(Sem);return  ret;}
		}
		if(outlen-trashLen*n>0)
		{
			ret=(Y_Write)((BYTE *)(InString+n*trashLen),Address+n*trashLen,(BYTE)(outlen-n*trashLen),ary1,DevicePath);
			if(ret!=0){sem_post(Sem);sem_close(Sem);return  ret;}

		}
		sem_post(Sem);
	}
	sem_close(Sem);
	return ret;
}

int YReadString(char *string ,short Address,int len,char * HKey,char *LKey,char *DevicePath )
{
	BYTE ary1[8];int n,trashLen=16;int ret=0;int total_len;//DWORD z,z1;int versionex;


	myconvert(HKey,LKey,ary1);

	total_len=Address+len;
	if(total_len>495){return -47;}

	sem_t * Sem =   sem_open("ex_sim",FLAGS , PERMS, 1);
	{
		sem_wait(Sem);
		for(n=0;n<len/trashLen;n++)
		{
			ret=(Y_Read)((BYTE*)(string+n*trashLen),Address+n*trashLen,trashLen,ary1,DevicePath);
			if(ret!=0)
			{
				sem_post(Sem);sem_close(Sem);
				return ret;
			}
		}
		if(len-trashLen*n>0)
		{
			ret=(Y_Read)((BYTE*)(string+n*trashLen),Address+n*trashLen,(len-trashLen*n),ary1,DevicePath);
			if(ret!=0)
			{
				sem_post(Sem);sem_close(Sem);
				return ret;
			}

		}
		sem_post(Sem);
	}
	sem_close(Sem);
	return ret;
}

void ByteArrayToHexString(BYTE *in_data,char * OutString,int len)
{
	memset(OutString,0,len*2);
	char temp[5];int n;
	for (n=1;n<=len;n++)
	{
		sprintf(temp,"%02X",*in_data);
		in_data++;
		strcat(OutString,temp);
	}
}

void HexStringToByteArray(char * InString,BYTE *in_data)
{
	int len=strlen(InString);
	char temp[5];int n;
	for (n=1;n<=len;n=n+2)
	{
		memset(temp,0,sizeof(temp));
		strncpy(temp,&InString[n-1],2);
		*in_data=(BYTE)HexToInt(temp);
		in_data++;
	}
}

void HexStringToByteArrayEx(char * InString,BYTE *in_data)
{
	int len=strlen(InString);
	char temp[5];int n;
	if(len>32)len=32;
	for (n=1;n<=len;n=n+2)
	{
		memset(temp,0,sizeof(temp));
		strncpy(temp,&InString[n-1],2);
		*in_data=(BYTE)HexToInt(temp);
		in_data++;
	}

}

int SetCal_2(char *Key,char *DevicePath)
{
	BYTE KeyBuf[16];
	memset(KeyBuf,0,16);
	HexStringToByteArrayEx(Key,KeyBuf);
	//ע�⣬�����ǵ�ַ������
	int ret;
	sem_t * Sem =   sem_open("ex_sim",FLAGS , PERMS, 1);
	sem_wait(Sem);
	ret=(NT_SetCal_2)(&KeyBuf[8],0,DevicePath);
	if(ret!=0)goto error1;
	ret=(NT_SetCal_2)(&KeyBuf[0],1,DevicePath);
error1:
	sem_post(Sem);
	sem_close(Sem);
	return ret;

}

int Cal(  BYTE *InBuf,BYTE *OutBuf,char *DevicePath)
{
	int ret;
	sem_t * Sem =   sem_open("ex_sim",FLAGS , PERMS, 1);
	sem_wait(Sem);
	ret=NT_Cal(InBuf,OutBuf,DevicePath);
	sem_post(Sem);
	sem_close(Sem);
	return ret;
}

int GetLen(char *InString)
{
	return strlen(InString)+1;
}

int EncString(  char *InString,char *OutString,char *DevicePath)
{

	int ret;int n;BYTE *outbuf;
	int len=strlen(InString)+1;if(len<8)len=8;
	outbuf=new BYTE[len];
	memset(outbuf,0,len);
	memcpy(outbuf,InString,strlen(InString)+1);
	for(n=0;n<=(len-8);n=n+8)
	{
		ret=Cal((BYTE *)&outbuf[n],&outbuf[n],DevicePath);
		if(ret!=0){delete [] outbuf;return ret;}
	}
	ByteArrayToHexString(outbuf,OutString,len);
	delete [] outbuf;
	return ret;
}

void  EncBySoft(   BYTE  *   aData,  BYTE   *   aKey   )
{
	const   unsigned   int   cnDelta   =   0x9E3779B9;
	register   unsigned   int   y   =   (   (   unsigned   int   *   )aData   )[0],   z   =   (   (   unsigned   int   *   )aData   )[1];
	register   unsigned   int   sum   =   0;
	unsigned   int   a   =   (   (   unsigned   int   *   )aKey   )[0],   b   =   (   (   unsigned   int   *   )aKey   )[1];
	unsigned   int   c   =   (   (   unsigned   int   *   )aKey   )[2],   d   =   (   (   unsigned   int   *   )aKey   )[3];
	int   n   =   32;

	while   (   n--   >   0   )
	{
		sum   +=   cnDelta;
		y   +=   ((   z   <<   4   )   +   a )  ^   (z   +   sum )  ^  ( (   z   >>   5   )   +   b);

		z   +=   ((   y   <<   4   )   +   c )  ^   (y   +   sum )  ^  ( (   y   >>   5   )   +   d);
	}
	(   (   unsigned   int   *   )aData   )[0]   =   y;
	(   (   unsigned   int   *   )aData   )[1]   =   z;
}

void  DecBySoft(    BYTE  *   aData,   BYTE   *   aKey   )
{
	const   unsigned   int   cnDelta   =   0x9E3779B9;
	register   unsigned   int   y   =   (   (   unsigned   int   *   )aData   )[0],   z   =   (   (   unsigned   int   *   )aData   )[1];
	register   unsigned   int   sum   =   0xC6EF3720;
	unsigned   int   a   =   (   (   unsigned   int   *   )aKey   )[0],   b   =   (   (   unsigned   int   *   )aKey   )[1];
	unsigned   int   c   =   (   (   unsigned   int   *   )aKey   )[2],   d   =   (   (   unsigned   int   *   )aKey   )[3];
	int   n   =   32;
	while   (   n--   >   0   )
	{
		z   -=  ( (   y   <<   4   )   +   c )  ^  ( y   +   sum  ) ^ (  (   y   >>   5   )   +   d);
		y   -=  ( (   z   <<   4   )   +   a )  ^  ( z   +   sum  ) ^ (  (   z   >>   5   )   +   b);
		sum   -=   cnDelta;
	}
	(   (   unsigned   int   *   )aData   )[0]   =   y;
	(   (   unsigned   int   *   )aData   )[1]   =   z;
}

int sWriteEx(  DWORD in_data,DWORD *out_data,char *DevicePath)
{
	int ret;
	sem_t * Sem =   sem_open("ex_sim",FLAGS , PERMS, 1);
	sem_wait(Sem);
	ret=(WriteDword)(&in_data,DevicePath);
	if(ret!=0)goto error1;
	ret=(ReadDword)(out_data,DevicePath);
error1:
	sem_post(Sem);
	sem_close(Sem);
	return ret;
}

int sWrite_2Ex(  DWORD in_data,DWORD *out_data,char *DevicePath)
{
	int ret;
	sem_t * Sem =   sem_open("ex_sim",FLAGS , PERMS, 1);
	sem_wait(Sem);
	ret=(WriteDword_2)(&in_data,DevicePath);
	if(ret!=0)goto error1;
	ret=(ReadDword)(out_data,DevicePath);
error1:
	sem_post(Sem);
	sem_close(Sem);
	return ret;
}

int sWriteEx_New(  DWORD in_data,DWORD *out_data,char *Path)
{
	int ret;
	sem_t * Sem =   sem_open("ex_sim",FLAGS , PERMS, 1);
	sem_wait(Sem);
	ret=(WriteDword_New)(&in_data,Path);
	if(ret!=0)goto error1;
	ret=(ReadDword)(out_data,Path);
error1:
	sem_post(Sem);
	sem_close(Sem);
	return ret;
}

int sWrite_2Ex_New(  DWORD in_data,DWORD *out_data,char *Path)
{
	int ret;
	sem_t * Sem =   sem_open("ex_sim",FLAGS , PERMS, 1);
	sem_wait(Sem);
	ret=(WriteDword_2_New)(&in_data,Path);
	if(ret!=0)goto error1;
	ret=(ReadDword)(out_data,Path);
error1:
	sem_post(Sem);
	sem_close(Sem);
	return ret;
}


void DecString(  char *InString,char *OutString,char *Key)
{
	int n;
	int len=strlen(InString)/2;
	BYTE KeyBuf[16];
	memset(KeyBuf,0,16);
	HexStringToByteArrayEx(Key,KeyBuf);
	HexStringToByteArray(InString,(BYTE *)OutString);
	for(n=0;n<=(len-8);n=n+8)
	{
		DecBySoft((BYTE *)&OutString[n],KeyBuf);
	}
	return ;
}

int ReSet(char *DevicePath)
{

	int ret;int Version;
	sem_t * Sem =   sem_open("ex_sim",FLAGS , PERMS, 1);
	sem_wait(Sem);
	ret=(GetVersion)(&Version,DevicePath);
	if(ret!=0)goto finish;
	ret=(NT_ReSet)(DevicePath);
	if(ret!=0)goto finish;
	ret=(NT_ReSet)(DevicePath);
	if(ret!=0)goto finish;
	ret=(NT_ReSet)(DevicePath);
	if(ret!=0)goto finish;
	ret=(NT_ReSet)(DevicePath);
finish:
	sem_post(Sem);
	sem_close(Sem);
	return ret;

}




int NT_GetVersionEx(int *version,char *DevicePath )
{
	int ret;
	sem_t * Sem =   sem_open("ex_sim",FLAGS , PERMS, 1);
	sem_wait(Sem);
	ret=(F_GetVerEx)(version,DevicePath);
	sem_post(Sem);
	sem_close(Sem);
	return ret;
}

int SetCal_New(char *Key,char *DevicePath)
{
	BYTE KeyBuf[16];
	memset(KeyBuf,0,16);
	HexStringToByteArrayEx(Key,KeyBuf);
	//ע�⣬�����ǵ�ַ������
	int ret;
	sem_t * Sem =   sem_open("ex_sim",FLAGS , PERMS, 1);
	sem_wait(Sem);
	ret=(NT_SetCal_New)(&KeyBuf[8],0,DevicePath);
	if(ret!=0)goto error1;
	ret=(NT_SetCal_New)(&KeyBuf[0],1,DevicePath);
error1:
	sem_post(Sem);
	sem_close(Sem);
	return ret;

}

int Cal_New(  BYTE *InBuf,BYTE *OutBuf,char *DevicePath)
{
	int ret;
	sem_t * Sem =   sem_open("ex_sim",FLAGS , PERMS, 1);
	sem_wait(Sem);
	ret=NT_Cal_New(InBuf,OutBuf,DevicePath);
	sem_post(Sem);
	sem_close(Sem);
	return ret;
}

int EncString_New(  char *InString,char *OutString,char *DevicePath)
{

	int ret;int n;BYTE *outbuf;
	int len=strlen(InString)+1;if(len<8)len=8;
	outbuf=new BYTE[len];
	memset(outbuf,0,len);
	memcpy(outbuf,InString,strlen(InString)+1);
	for(n=0;n<=(len-8);n=n+8)
	{
		ret=Cal_New((BYTE *)&outbuf[n],&outbuf[n],DevicePath);
		if(ret!=0){delete [] outbuf;return ret;}
	}
	ByteArrayToHexString(outbuf,OutString,len);
	delete [] outbuf;
	return ret;
}

void SwitchByte2Char(char *outstring,BYTE *inbyte,int inlen)
{
	int n;char temp[3];
	memset(outstring,0,ECC_MAXLEN*2);
	for(n=0;n<inlen;n++)
	{
		sprintf(temp,"%02X",inbyte[n]);
		strcat(outstring,temp);
	}

}

void SwitchChar2Byte(char *instring,BYTE *outbyte)
{
	int n;char temp[3];
	int inlen=strlen(instring)/2;
	for(n=0;n<inlen;n++)
	{
		temp[2]=(char)0;
		strncpy(temp,&instring[n*2],2);
		*outbyte=HexToInt(temp);
		outbyte++;
	}

}

int YT_GenKeyPair(char* PriKey,char *PubKeyX,char *PubKeyY,char *DevicePath)
{

	int ret;BYTE b_PriKey[ECC_MAXLEN],b_PubKey[ECC_MAXLEN*2+1];//���е�һ���ֽ��Ǳ�־λ������
	sem_t * Sem =   sem_open("ex_sim",FLAGS , PERMS, 1);
	sem_wait(Sem);
	ret=(NT_GenKeyPair)(b_PriKey,b_PubKey,DevicePath);
	sem_post(Sem);
	sem_close(Sem);
	memset(PriKey,0,2*ECC_MAXLEN+1);
	memset(PubKeyX,0,2*ECC_MAXLEN+1);
	memset(PubKeyY,0,2*ECC_MAXLEN+1);
	SwitchByte2Char(PriKey,b_PriKey,ECC_MAXLEN);
	SwitchByte2Char(PubKeyX,&b_PubKey[1],ECC_MAXLEN);
	SwitchByte2Char(PubKeyY,&b_PubKey[1+ECC_MAXLEN],ECC_MAXLEN);
	return ret;

}

int Set_SM2_KeyPair(char *PriKey,char *PubKeyX,char *PubKeyY,char *sm2UserName,char *DevicePath )
{

	int ret;BYTE b_PriKey[ECC_MAXLEN],b_PubKeyX[ECC_MAXLEN],b_PubKeyY[ECC_MAXLEN];
	SwitchChar2Byte(PriKey,b_PriKey);
	SwitchChar2Byte(PubKeyX,b_PubKeyX);
	SwitchChar2Byte(PubKeyY,b_PubKeyY);
	sem_t * Sem =   sem_open("ex_sim",FLAGS , PERMS, 1);
	sem_wait(Sem);
	ret=(NT_Set_SM2_KeyPair)(b_PriKey,b_PubKeyX,b_PubKeyY,sm2UserName,DevicePath);
	sem_post(Sem);
	sem_close(Sem);
	return ret;

}

int Get_SM2_PubKey(char *PubKeyX,char *PubKeyY,char *sm2UserName,char *DevicePath)
{

	int ret;BYTE b_PubKeyX[ECC_MAXLEN],b_PubKeyY[ECC_MAXLEN];
	sem_t * Sem =   sem_open("ex_sim",FLAGS , PERMS, 1);
	sem_wait(Sem);
	ret=(NT_Get_SM2_PubKey)(b_PubKeyX,b_PubKeyY,sm2UserName,DevicePath);
	sem_post(Sem);
	sem_close(Sem);
	memset(PubKeyX,0,2*ECC_MAXLEN+1);
	memset(PubKeyY,0,2*ECC_MAXLEN+1);
	SwitchByte2Char(PubKeyX,b_PubKeyX,ECC_MAXLEN);
	SwitchByte2Char(PubKeyY,b_PubKeyY,ECC_MAXLEN);
	return ret;

}

int SM2_EncBuf(BYTE *InBuf,BYTE *OutBuf,int inlen,char *DevicePath)
{

	int ret,temp_inlen;
	sem_t * Sem =   sem_open("ex_sim",FLAGS , PERMS, 1);
	sem_wait(Sem);
	while(inlen>0)
	{
		if(inlen>MAX_ENCLEN)
			temp_inlen=MAX_ENCLEN;
		else
			temp_inlen=inlen;
		ret=(NT_SM2_Enc)(InBuf,OutBuf,temp_inlen,DevicePath);
		if(ret!=0)goto err;
		inlen=inlen-MAX_ENCLEN;
		InBuf=InBuf+MAX_ENCLEN;
		OutBuf=OutBuf+MAX_DECLEN;
	}
err:
	sem_post(Sem);
	sem_close(Sem);
	return ret;

}

int SM2_DecBuf(BYTE *InBuf,BYTE *OutBuf,int inlen,char* pin,char *DevicePath)
{

	int ret,temp_inlen;
	sem_t * Sem =   sem_open("ex_sim",FLAGS , PERMS, 1);
	sem_wait(Sem);
	while(inlen>0)
	{
		if(inlen>MAX_DECLEN)
			temp_inlen=MAX_DECLEN;
		else
			temp_inlen=inlen;
		ret=(NT_SM2_Dec)(InBuf,OutBuf,temp_inlen,pin,DevicePath);
		if(ret!=0)goto err;
		inlen=inlen-MAX_DECLEN;
		InBuf=InBuf+MAX_DECLEN;
		OutBuf=OutBuf+MAX_ENCLEN;
	}
err:
	sem_post(Sem);
	sem_close(Sem);
	return ret;

}

int SM2_EncString(char *InString,char *OutString,char *DevicePath)
{

	int inlen=strlen(InString)+1;
	int outlen=(inlen/(MAX_ENCLEN)+1)*SM2_ADDBYTE+inlen;
	BYTE *OutBuf=new BYTE[outlen];
	BYTE *p=OutBuf;
	int ret,temp_inlen;
	sem_t * Sem =   sem_open("ex_sim",FLAGS , PERMS, 1);
	sem_wait(Sem);
	while(inlen>0)
	{
		if(inlen>MAX_ENCLEN)
			temp_inlen=MAX_ENCLEN;
		else
			temp_inlen=inlen;
		ret=(NT_SM2_Enc)((BYTE *)InString,OutBuf,temp_inlen,DevicePath);
		if(ret!=0)goto err;
		inlen=inlen-MAX_ENCLEN;
		InString=InString+MAX_ENCLEN;
		OutBuf=OutBuf+MAX_DECLEN;
	}
err:
	memset(OutString,0,2*outlen+1);
	ByteArrayToHexString(p,OutString,outlen);
	sem_post(Sem);
	sem_close(Sem);
	delete [] p;
	return ret;

}

int SM2_DecString(char *InString,char *OutString,char* pin,char *DevicePath)
{

	int inlen=strlen(InString)/2;
	BYTE *InByte=new BYTE[inlen];
	BYTE *p=InByte;
	int ret,temp_inlen;
	SwitchChar2Byte(InString,InByte);
	sem_t * Sem =   sem_open("ex_sim",FLAGS , PERMS, 1);
	sem_wait(Sem);
	while(inlen>0)
	{
		if(inlen>MAX_DECLEN)
			temp_inlen=MAX_DECLEN;
		else
			temp_inlen=inlen;
		ret=(NT_SM2_Dec)((BYTE *)InByte,(BYTE *)OutString,temp_inlen,pin,DevicePath);
		if(ret!=0)goto err;
		inlen=inlen-MAX_DECLEN;
		InByte=InByte+MAX_DECLEN;
		OutString=OutString+MAX_ENCLEN;
	}
err:
	sem_post(Sem);
	sem_close(Sem);
	delete [] p;
	return ret;

}

int YtSetPin(char *old_pin,char *new_pin,char *DevicePath )
{
	int ret;
	sem_t * Sem =   sem_open("ex_sim",FLAGS , PERMS, 1);
	sem_wait(Sem);
	ret=(NT_Set_Pin)(old_pin,new_pin,DevicePath);
	sem_post(Sem);
	sem_close(Sem);
	return ret;
}

void SwitchBigInteger2Byte(char *instring,BYTE *outbyte,int *outlen)
{
	int n;char temp[3];
	int inlen=strlen(instring)/2;
	*outlen=0x20;

	for(n=0;n<inlen;n++)
	{
		temp[2]=(char)0;
		strncpy(temp,&instring[n*2],2);
		*outbyte=(BYTE)HexToInt(temp);
		outbyte++;
	}

}


int GetChipID( char *OutChipID,char *DevicePath)
{
	int ret;BYTE b_OutChipID[16];
	sem_t * Sem =   sem_open("ex_sim",FLAGS , PERMS, 1);
	sem_wait(Sem);
	ret=(NT_GetChipID)(b_OutChipID,DevicePath);
	sem_post(Sem);
	sem_close(Sem);
	memset(OutChipID,0,33);
	ByteArrayToHexString(b_OutChipID,OutChipID,16);
	return ret;
}


void EnCode(   BYTE  * InData,BYTE  * OutData,  char *Key  )
{
	BYTE KeyBuf[16];
	memset(KeyBuf,0,16);
	HexStringToByteArrayEx(Key,KeyBuf);
	memcpy(OutData,InData,8);
	EncBySoft(OutData,KeyBuf);
}

void DeCode(   BYTE  * InData, BYTE  * OutData, char *Key  )
{
	BYTE KeyBuf[16];
	memset(KeyBuf,0,16);
	HexStringToByteArrayEx(Key,KeyBuf);
	memcpy(OutData,InData,8);
	DecBySoft(OutData,KeyBuf);
}

void StrDec(  char *InString,char *OutString,char *Key)
{
	int n;
	int len=strlen(InString)/2;
	BYTE KeyBuf[16];
	memset(KeyBuf,0,16);
	HexStringToByteArrayEx(Key,KeyBuf);
	HexStringToByteArray(InString,(BYTE *)OutString);
	for(n=0;n<=(len-8);n=n+8)
	{
		DecBySoft((BYTE *)&OutString[n],KeyBuf);
	}
	return ;
}

void StrEnc(  char *InString,char *OutString,char *Key)
{
	int n;BYTE *outbuf;
	int len=strlen(InString)+1;if(len<8)len=8;
	BYTE KeyBuf[16];
	memset(KeyBuf,0,16);
	HexStringToByteArrayEx(Key,KeyBuf);
	outbuf=new BYTE[len];
	memset(outbuf,0,len);
	memcpy(outbuf,InString,strlen(InString)+1);
	for(n=0;n<=(len-8);n=n+8)
	{
		EncBySoft(&outbuf[n],KeyBuf);
	}
	ByteArrayToHexString(outbuf,OutString,len);
	delete [] outbuf;
}

int SetHidOnly(  BOOL IsHidOnly,char *InPath)
{
	int ret;
	sem_t * Sem =   sem_open("ex_sim",FLAGS , PERMS, 1);
	sem_wait(Sem);
	ret=NT_SetHidOnly(IsHidOnly,InPath);
	sem_post(Sem);
	sem_close(Sem);
	return ret;
}
int SetUReadOnly( char *InPath)
{
	int ret;
	sem_t * Sem =   sem_open("ex_sim",FLAGS , PERMS, 1);
	sem_wait(Sem);
	ret=NT_SetUReadOnly(InPath);
	sem_post(Sem);
	sem_close(Sem);
	return ret;
}

void CloseUsbHandle(char *DevicePath )
{
	char *p;
	if(strlen(DevicePath)>1)
	{
		p=strchr(DevicePath,':');
		if(p==NULL){return ;}
		p=p+1;
		char *stop;/*2017-1-4*/
		long long i_KeyPath=strtoll(p,&stop,10);/*2017-1-4*/
		libusb_device_handle * DevHandle =(libusb_device_handle *)i_KeyPath;
		libusb_close(DevHandle);
	}
}

