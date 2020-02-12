#ifndef KEYDEF_H
#define KEYDEF_H


#define ErrlibusbClaimInterface -70
#define ErrlibusbInit -99
#define ErrGetDevList -98
#define ErrGetDevDesc -97
#define NotFoundKey -92
#define ErrSendData -93
#define ErrGetData  -94
#define EEPROMOVERCONST -22 //���������ĵ�ַ����ָ���ķ�Χ���������ܳ���EEPROM_LEN-4�����㲻�ܳ���EEPROM_LEN-8
#define NOTOPENFILE -31//���ܴ��ļ�
#define NOTREADFILE -32 //���ܶ�ȡ�ļ�
#define NOTWRITEFILE -42 //����д�ļ�����鿴�ļ��Ƿ���ֻ�����ԣ��������ļ���
#define TRANFSERFAIL  -45 //��������ʧ��
#define ERRWRITEPASSWORD -47 //д���벻��ȷ
#define ERRREADPASSWORD -48 //�����벻��ȷ
#define EPPROMOVERADD  -49 //��дEEPROMʱ����ַ���
#define USBStatusFail -50  //USB����ʧ�ܣ�������û���ҵ���ص�ָ��
#define OPENUSBFAIL  -51  //��USB�ļ����ʧ��
#define ENCERROR   -52 //ʹ�ü����������Զ�����ʽ�����ɼ��ܴ���ʱ��������
#define NOUSBORDRIVER   -53 //�޷���usb�豸��������������û�а�װ��û�в����������
#define LESSCOUNT      -55  //Ҫ���ܵ���������8���ֽ�
#define GETVARERROR   -56   //���ؽ����������
#define NotVaildFile      -65  //������Ч��BIN�ļ�
#define ERRORKEY      -66  //����д���������ԭ������Կ����ȷ
#define OVERLEN    -67
#define SETCMPFAIL -68  //USB����ʧ��
#define FAILSetupDiGetClassDevs  -90005
#define FAILSetupDiGetDeviceInterfaceDetail -90006
#define NOUSBKEY   -92 //�Ҳ������ܹ���
#define ErrlibusbOpen -80
#define ErrAddrOver    -81
#define ErrWritePWD   -82
#define ErrReadPWD    -83
#define ErrResult -20

#define FAILEDGENKEYPAIR -21;
#define FAILENC          -22
#define FAILDEC          -23
#define FAILPINPWD  	 -24

#define SM2_ADDBYTE 97//���ܺ�����ݻ����ӵĳ���
#define MAX_ENCLEN 128 //���ļ��ܳ��ȷ���
#define MAX_DECLEN  (MAX_ENCLEN+SM2_ADDBYTE) //���Ľ��ܳ��ȷ���
#define USERID_LEN	  80//�����û���ݳ���


#define ECC_MAXLEN 	  32
#define PIN_LEN 	  16
#define MAX_LEN 2031

#define ReportLen_Usua 0x15
#define ReportLen_Smart 257

//��������
#define GETVERSION      0x01
#define GETID      		0x02
#define S_MYENC         0X03
#define S_MYDEC         0X04
#define GETVEREX        0x05
#define SETCAL			0x06
#define	SETID			0X07
#define CAL_TEA         0x08
#define SET_TEAKEY      0x09
#define CAL_TEA_2       0x0c
#define SET_TEAKEY_2    0x0d
#define READBYTE   		0x10
#define WRITEBYTE     	0x11
#define YTREADBUF_2K		0x10
#define YTWRITEBUF_2K      0x11
#define YTREADBUF		0x12
#define YTWRITEBUF      0x13
#define MYRESET         0x20
#define YTREBOOT   		0x24
#define SET_ECC_PARA    0x30
#define GET_ECC_PARA	0x31
#define SET_ECC_KEY     0x32
#define GET_ECC_KEY     0x33
#define MYENC			0x34
#define MYDEC			0X35
#define SET_PIN			0X36
#define GEN_KEYPAIR     0x37
#define YTSIGN          0x51
#define YTVERIFY        0x52
#define GET_CHIPID      0x53
#define YTSIGN_2        0x54
#define WRITESN			0XCE
#define GETSN			0x0F
#define WRITESN			0XCE
#define	SETHIDONLY	    0x55
#define SETREADONLY		0x56





#endif // KEYDEF_H
