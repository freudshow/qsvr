#ifndef _MD5_H_
#define _MD5_H_

#include "basedef.h"

#define INT8U u8
#define UINT8 u8
#define UINT32 u32

typedef enum
{
    FR_OK = 0,				/* (0) Succeeded */
    FR_DISK_ERR,			/* (1) A hard error occurred in the low level disk I/O layer */
    FR_INT_ERR,				/* (2) Assertion failed */
    FR_NOT_READY,			/* (3) The physical drive cannot work */
    FR_NO_FILE,				/* (4) Could not find the file */
    FR_NO_PATH,				/* (5) Could not find the path */
    FR_INVALID_NAME,		/* (6) The path name format is invalid */
    FR_DENIED,				/* (7) Access denied due to prohibited access or directory full */
    FR_EXIST,				/* (8) Access denied due to prohibited access */
    FR_INVALID_OBJECT,		/* (9) The file/directory object is invalid */
    FR_WRITE_PROTECTED,		/* (10) The physical drive is write protected */
    FR_INVALID_DRIVE,		/* (11) The logical drive number is invalid */
    FR_NOT_ENABLED,			/* (12) The volume has no work area */
    FR_NO_FILESYSTEM,		/* (13) There is no valid FAT volume */
    FR_MKFS_ABORTED,		/* (14) The f_mkfs() aborted due to any parameter error */
    FR_TIMEOUT,				/* (15) Could not get a grant to access the volume within defined period */
    FR_LOCKED,				/* (16) The operation is rejected according to the file sharing policy */
    FR_NOT_ENOUGH_CORE,		/* (17) LFN working buffer could not be allocated */
    FR_TOO_MANY_OPEN_FILES,	/* (18) Number of open files > _FS_SHARE */
    FR_INVALID_PARAMETER	/* (19) Given parameter is invalid */
} FRESULT;

#define R_memset(x, y, z) memset(x, y, z)
#define R_memcpy(x, y, z) memcpy(x, y, z)
#define R_memcmp(x, y, z) memcmp(x, y, z)

#ifdef	__cplusplus
extern "C" {
#endif

typedef unsigned char *POINTER;

/* MD5 context. */
typedef struct
{
    /* state (ABCD) */
    /*四个32bits数，用于存放最终计算得到的消息摘要。当消息长度〉512bits时，也用于存放每个512bits的中间结果*/
    UINT32 state[4];
    /* number of bits, modulo 2^64 (lsb first) */
    /*存储原始信息的bits数长度,不包括填充的bits，最长为 2^64 bits，因为2^64是一个64位数的最大值*/
    UINT32 count[2];

    /* input buffer */
    /*存放输入的信息的缓冲区，512bits*/
    unsigned char buffer[64];
} MD5_CTX;

void MD5Init(MD5_CTX *);
void MD5Update(MD5_CTX *, unsigned char *, unsigned int);
void MD5Final(unsigned char [16], MD5_CTX *);
void testmd5(void);
FRESULT MD5File(char *filename, INT8U *md5code);

#ifdef	__cplusplus
}
#endif

#endif /* _MD5_H_ */
