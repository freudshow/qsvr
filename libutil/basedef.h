#ifndef BASEDEF_H
#define BASEDEF_H

#ifdef __cplusplus
extern "C" {
#endif


typedef unsigned char           boolean;      /* bool value, 0-false, 1-true       */
typedef unsigned char           u8;           /* Unsigned  8 bit quantity          */
typedef signed   char           s8;           /* Signed    8 bit quantity          */
typedef unsigned short          u16;          /* Unsigned 16 bit quantity          */
typedef signed   short          s16;          /* Signed   16 bit quantity          */
typedef unsigned int            u32;          /* Unsigned 32 bit quantity          */
typedef signed   int            s32;          /* Signed   32 bit quantity          */
typedef unsigned long long     	u64;          /* Unsigned 64 bit quantity   	   */
typedef signed long long  		s64;          /* Unsigned 64 bit quantity          */
typedef float                   fp32;         /* Single precision floating point   */
typedef double                  fp64;         /* Double precision floating point   */

#define TRUE        1       //作为比较的结果, 真; 或者函数的返回值, 无错误发生
#define FALSE       0       //作为比较的结果, 假; 或者函数的返回值, 有错误发生

#define POLY		0xA001	//CRC16校验中的生成多项式

#define HEX_TO_BCD(x) (((x)/0x0A)*0x10+((x)%0x0A))
#define BCD_TO_HEX(x) (((x)/0x10)*0x0A+((x)%0x10))
#define ASCII_TO_HEX(c) (((c) >='0' && (c) <='9')?((c)-'0'):(((c)>='A'&&(c)<='F')?((c)-'A'+10):(((c)>='a'&&c<='f')?((c)-'a'+10):0)))
#define isDigit(c) ((unsigned) ((c)-'0') < 10)
#define isHex(c) (((unsigned) ((c)-'0') < 10) || ((unsigned) ((c)-'A') < 6) || ((unsigned) ((c)-'a') < 6) )
#define isDelim(c) (c == ' ' || c == '\n' || c == 't' || c == '\r')
#define isCOMBCD(x) (((((x)>>4)&0x0F) <= 9u) && (((x)&0x0F) <= 9u))
#define ELEM_NUM(array)         (sizeof(array)/sizeof(array[0]))

#define HEX_TO_ASCII(x) ((x<=0x09)?(x+0x30):(x+0x37))

#define U8_TO_ASCII_H(x) HEX_TO_ASCII(((x)&0x0F))
#define U8_TO_ASCII_L(x) HEX_TO_ASCII(((x)&0x0F))
#define BIT_N(byte, n) (((byte)>>(n))&0x01)
#define	NELEM(array)	(sizeof(array)/sizeof(array[0]))

#define	FILE_LINE		__FILE__,__FUNCTION__,__LINE__

#define UNUSEDV(v)      (void)v

#define	DEBUG_BUFF(data, dataSize)		debugBuf(FILE_LINE, data, dataSize)
#define	DEBUG_TIME_LINE(format, ...)	debugToStderr(FILE_LINE, format, ##__VA_ARGS__)
#define DEBUG_TO_FILE(fname, format, ...)	debugToFile(fname, FILE_LINE, format, ##__VA_ARGS__)

#define FILE_MAX_SIZE   (5*1024*1024)//5M

#define TIME_OUT		2000

/*************************************************************
 * swap two number(u8/s8, u16/s16, u32/s32).
 * note: 'a' and 'b' must be the same datatype.
 * if a == b, then this macro do not swap them.
 * ***********************************************************/
#define SWAP(a, b)      do{\
                            (a) = (a)^(b);\
                            (b) = (a)^(b);\
                            (a) = (a)^(b);\
                        }while(0)

#define min(X, Y)  ((X) < (Y) ? (X) : (Y))
#define max(X, Y)  ((X) > (Y) ? (X) : (Y))

#define max_typeof(x, y)   ({\
                        typeof(x) _x = x;\
                        typeof(y) _y = y;\
                        _x > _y ? _x : _y;\
                    })

#define auto_max(x,y) ({\
                        __auto_type _x = x;\
                        __auto_type _y = y;\
                        _x > _y ? _x : _y;\
                    })


#define NOT_USED(x) (void)(x)

#define CONNECT_THREAD(object, run, thread)     do {\
                                                    (object)->moveToThread(thread);\
                                                    connect((thread), SIGNAL(started()), (object), SLOT(run()));\
                                                    connect((object), SIGNAL(finished()), (thread), SLOT(quit()));\
                                                    connect((object), SIGNAL(finished()), (object), SLOT(deleteLater()));\
                                                    connect((thread), SIGNAL(finished()), (thread), SLOT(deleteLater()));\
                                                    (thread)->start();\
                                                }while(0)

#define SET_POINTER_NULL(p)	((p) = Q_NULLPTR);

#define RELEASE_POINTER_RESOURCE(p)	if (Q_NULLPTR != (p)){\
                                            delete (p);\
                                            p = Q_NULLPTR;\
                                    }

#define RELEASE_TIMER_RESOURCE(pt)		if (Q_NULLPTR != (pt)){\
                                            if((pt)->isActive())\
                                                (pt)->stop();\
                                            delete (pt);\
                                            pt = Q_NULLPTR;\
                                        }

#define RELEASE_COM_RESOURCE(pcom)		if(Q_NULLPTR != (pcom)){\
                                            if((pcom)->isOpen()){\
                                                (pcom)->close();\
                                            }\
                                            delete (pcom);\
                                            pcom = Q_NULLPTR;\
                                        }

typedef struct bcdCode {
    u8  low     :4;
    u8  high    :4;
}bcd_s;

extern void debugBuf(const char* file, const char* func, u32 line, u8* buf, u32 bufSize);
extern void debugToStderr(const char* file, const char* func, u32 line, const char *fmt, ...);
extern void debugToFile(const char* fname, const char* file, const char* func, u32 line, const char *fmt,...);
extern void printBuf(u8* buf, u32 bufSize, u8 space, u8 inverse);
extern u16 crc16TblDrv(const u8 *nData, int wLength);
extern u16 calcModRtuCRC(u8 *buf, int len);
extern u16 fcs16(unsigned char *cp, int  len);
extern void add33(u8* buf, int bufSize);
extern void minus33(u8* buf, int bufSize);
extern u8 chkSum(u8* buf, int bufSize);
extern void inverseArray(u8* buf, u16 bufSize);

#ifdef __cplusplus
}
#endif

#endif // BASEDEF_H
