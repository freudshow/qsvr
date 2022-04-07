#ifndef BASEDEF_H
#define BASEDEF_H

#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>

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

#define	NELEM(array)	(sizeof(array)/sizeof(array[0]))//求一个数组的元素个数

#define	FILE_LINE		__FILE__,__FUNCTION__,__LINE__  //调试打印文件, 函数, 行号

#define UNUSEDV(v)      (void)v                         //消除变量已定义未使用警告
#define MIN_TTU(X, Y)  ((X) < (Y) ? (X) : (Y))          //求二者中的最小值
#define MAX_TTU(X, Y)  ((X) > (Y) ? (X) : (Y))          //求二者中的最大值
#define LOG_SIZE	(5*1024*1024)						//日志文件最大长度
#define LOG_COUNT	10                                  //日志文件最大个数
#define DEBUG_OUT_TO_FILES                              //打印日志到文件开关
#define DEBUG_OUT_TO_SCREEN                           //打印日志到屏幕开关

#ifdef DEBUG_OUT_TO_FILES

//格式化打印日志, 带报文输出
#define DEBUG_BUFFFMT_TO_FILE(fname, buf, bufSize, format, ...) do {\
                                                                        FILE *fp = fopen(fname, "a+");\
                                                                        if (fp != NULL) {\
                                                                            debugBufFormat2fp(fp, FILE_LINE, buf, bufSize, format, ##__VA_ARGS__);\
                                                                        }\
                                                                        logLimit(fname, LOG_SIZE, LOG_COUNT);\
                                                                    } while(0)

//格式化打印日志
#define DEBUG_TO_FILE(fname, format, ...)	do {\
                                                FILE *fp = fopen(fname, "a+");\
                                                if (fp != NULL) {\
                                                    debugBufFormat2fp(fp, FILE_LINE, NULL, 0, format, ##__VA_ARGS__);\
                                                }\
                                                logLimit(fname, LOG_SIZE, LOG_COUNT);\
                                            } while(0)

#else

#define DEBUG_BUFFFMT_TO_FILE(filename, buf, bufSize, format, ...)
#define DEBUG_TO_FILE(fname, format, ...)

#endif

#ifdef DEBUG_OUT_TO_SCREEN
#define	DEBUG_BUFF_FORMAT(buf, bufSize, format, ...)	debugBufFormat2fp(stdout, FILE_LINE, (char*)buf, (int)bufSize, format, ##__VA_ARGS__)
#define	DEBUG_TIME_LINE(format, ...)	DEBUG_BUFF_FORMAT(NULL, 0, format, ##__VA_ARGS__)
#else
#define	DEBUG_BUFF_FORMAT(data, dataSize, format, ...)
#define	DEBUG_TIME_LINE(format, ...)
#endif

/**
 * 声明一个列表
 */
#define	A_LIST_OF(type)					    \
    struct {					            \
        u32  capacity; /*当前列表的最大容量*/		\
        u32  count;    /*当前列表的元素个数*/		\
        u32  idx;	   /*当前元素索引*/      	\
        type *list;	   /*列表*/			    \
        void (*free)(void *);/*释放列表*/     \
    }

/**
 * 初始化一个列表
 */
#define INIT_LIST(vlist, type, quantity, freeFunc)   \
    do {\
        vlist.capacity = 0;\
        vlist.idx = 0;\
        vlist.list = (type*)malloc(quantity * sizeof(type));\
        if(vlist.list != NULL) {\
            vlist.capacity = quantity;\
        }\
        vlist.free = freeFunc;\
    } while(0)

/**
 * 列表扩容
 */
#define EXTEND_LIST(vlist, type, delta)   \
    do {\
        type *p = (type*)malloc((vlist.count + delta)*sizeof(type));\
        if(p != NULL) {\
            memcpy(p, vlist.list, vlist.count*sizeof(type));\
            vlist.free(vlist.list);\
            vlist.list = p;\
            vlist.capacity += delta;\
        }\
    }while(0)



/**********byte macros***********/
#define HEX_TO_BCD(x) (((x)/0x0A)*0x10+((x)%0x0A))
#define BCD_TO_HEX(x) (((x)/0x10)*0x0A+((x)%0x10))

#define MERGE_TWO_BYTE(byt_h,byt_l)	(((u16)((byt_h)<<8))+byt_l)
#define LOW_BYTE_OF(dbyte)				((u8)((u16)(dbyte)))
#define HIGH_BYTE_OF(dbyte)			((u8)(((u16)(dbyte))>>8))

/*************************************************************
 * convert hex char to ascii code
 * note: 'x' must be in interval [0x00, 0x0F]
 * example: HEX_TO_ASCII(0xc), return 'C'
 * ***********************************************************/
#define HEX_TO_ASCII(x)				((x<=0x09)?(x+0x30):(x+0x37))

/*************************************************************
 * convert ascii code to hex char
 * note: 'x' must be in interval:
 * {['0', '9'] U ['a', 'f'] U ['A', 'F']}
 * * example: ASCII_TO_HEX('4'), return 0x04,
 * 			  ASCII_TO_HEX('e'), return 0x0e,
 * 			  ASCII_TO_HEX('B'), return 0x0B, etc.
 * ***********************************************************/
#define ASCII_TO_HEX(c)				((c >='0' && c <='9') ? (c-'0'): (\
                                            (c>='A'&&c<='F') ? (c-'A'+10): (\
                                                    (c>='a'&&c<='f')?(c-'a'+10):0\
                                                )\
                                            )\
                                        )

/*************************************************************
 * convert BCD char to ascii code
 * example: BCD_TO_ASCII_H(0x56), return '5',
 * 			BCD_TO_ASCII_L(0x56), return '6'.
 * ***********************************************************/
#define BCD_TO_ASCII_H(x)			HEX_TO_ASCII((((x)>>4)&0x0F) + 0x30)
#define BCD_TO_ASCII_L(x)			HEX_TO_ASCII(((x)&0x0F) + 0x30)

/*************************************************************
 * get n binary of byte, n is 1 based.
 * example: BIT_N(0b10010010, 5), return 1,
 * 			BIT_N(0b10010010, 1), return 0.
 * ***********************************************************/
#define BIT_N(byte, n)				(((byte)>>(n))&0x01)

/*************************************************************
 * swap two number(u8/s8, u16/s16, u32/s32).
 * note: 'a' and 'b' must be the same datatype.
 * But if the two variables located in the same memory
 * address, they will be set to 0.
 * So, if a's address equals b's address, do not use XOR to
 * swap them.
 * ***********************************************************/
#define SWAP(a, b)      do{\
                            if (&a != &b) {\
                                (a) = (a)^(b);\
                                (b) = (a)^(b);\
                                (a) = (a)^(b);\
                            }\
                        }while(0)

//#define isDigit(c) ((unsigned) ((c)-'0') < 10)
//#define isHex(c) (((unsigned) ((c)-'0') < 10) || ((unsigned) ((c)-'A') < 6) || ((unsigned) ((c)-'a') < 6) )
#define isDelim(c) (c == ' ' || c == '\n' || c == 't' || c == '\r')

//计算环形队列的下一个元素的索引值
#define GET_NEXT_INDEX(index, capacity)	( ( (index)+1 ) % (capacity) )

#define MQ_MSG_MAX_COUNT	10  //消息队列最大个数
#define MQ_MSG_MAX_SIZE		512 //消息队列最大长度

/*
 * 压缩BCD码, 仅对小端设备有效
 */
typedef struct bcdCode {
    u8 low :4;				//BCD码的低4位
    u8 high :4;				//BCD码的高4位
} bcd_s;


//#define min(X, Y)  ((X) < (Y) ? (X) : (Y))
//#define max(X, Y)  ((X) > (Y) ? (X) : (Y))



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

extern void debugBufFormat2fp(FILE *fp, const char *file, const char *func,
                              int line, char *buf, int len, const char *fmt, ...);
extern int logLimit(const char *fname, int logsize, int logCount);
extern void getBufString(char *buf, int len, char *out);
extern u16 crc16TblDrv(const u8 *nData, int wLength);
extern u16 calcModRtuCRC(u8 *buf, int len);
extern u16 fcs16(unsigned char *cp, int len);
extern void add33(u8 *buf, int bufSize);
extern void minus33(u8 *buf, int bufSize);
extern u8 chkSum(u8 *buf, u16 bufSize);
extern void inverseArray(u8 *buf, u16 bufSize);
extern void debugBuf(const char* file, const char* func, u32 line, u8* buf, u32 bufSize);
extern void debugToStderr(const char* file, const char* func, u32 line, const char *fmt, ...);
extern void debugToFile(const char* fname, const char* file, const char* func, u32 line, const char *fmt,...);
extern void printBuf(u8* buf, u32 bufSize, u8 space, u8 inverse);
extern u8 xorSum(u8* buf, int bufSize);
extern u16 crc16continue(u8 *buf, int len, u16 crc, u8 lastCrc);
#ifdef __cplusplus
}
#endif

#endif // BASEDEF_H
