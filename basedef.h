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

#define TRUE        0x01
#define FALSE       !TRUE

//698.45扩展数据类型-------------------------------------
typedef unsigned char MAC_698;	//数据安全MAC
typedef unsigned char RN_698;	//随机数
typedef unsigned short OI_698;	//对象标识

#ifdef __cplusplus
}
#endif

#endif // BASEDEF_H
