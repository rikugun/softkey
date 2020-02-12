#ifndef UKEY_H
#define UKEY_H

#define DATATOOLEN	-3
#define ADDRERROR -4
#define USER_WRITE_PWD_ERROR -5
#define READ_PWD_ERROR -6
#define WRITE_PWD_ERROR -7
#define PinNotOpen -8
#define USER_READ_PWD_ERROR -9
#define NoASPI       -901
#define CANTLOADLIB  -902 
#define  CANTGETFUNC  -903

#define CANOPENEVENT  -905

#define DATAISNULL    -909
#define ALLOCMEMORYERROR -3000
#define ISNULL         -911

#define DISK_INF_LEN 0X24
#define SHORTTIMEOUT  1L*100L
#define SCSI_MSPGCD_RETALL 0x3f
#define DISK_SENSE_LEN 12

const unsigned char DISK_INF[DISK_INF_LEN]=                                      //¥≈≈Ã–≈œ¢
{
  0, 0x80, 
 0x00, 0x01, 0x1f, 0x00, 0x00, 0x00, 'i','K', 'e', 'y', '.', 0, 0, 0,
 'F','2', 'k', 'K', 'e', 'y', 'C', 'o', '.', ',', 0, 0, 0, 0, 0, 0,                                       
 '8', 0x2e, 0x30, 0x30
};


#endif // UKEY_H
