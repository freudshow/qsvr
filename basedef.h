#ifndef BASEDEF_H
#define BASEDEF_H

#ifdef __cplusplus
extern "C" {
#elif


typedef unsigned char  BOOLEAN;
typedef unsigned char  INT8U;                    /* Unsigned  8 bit quantity                           */
typedef signed   char  INT8S;                    /* Signed    8 bit quantity                           */
typedef unsigned short INT16U;                   /* Unsigned 16 bit quantity                           */
typedef signed   short INT16S;                   /* Signed   16 bit quantity                           */
typedef unsigned int   INT32U;                   /* Unsigned 32 bit quantity                           */
typedef signed   int   INT32S;                   /* Signed   32 bit quantity                           */
typedef unsigned long long     	INT64U;          /* Unsigned 64 bit quantity   						   */
typedef signed long long  		INT64S;          /* Unsigned 64 bit quantity                           */
typedef float          FP32;                     /* Single precision floating point                    */
typedef double         FP64;                     /* Double precision floating point                    */

#define TRUE 1
#define FALSE !TRUE

//698.45扩展数据类型-------------------------------------
typedef unsigned char MAC_698;	//数据安全MAC
typedef unsigned char RN_698;	//随机数
typedef unsigned short OI_698;	//对象标识

#ifdef __cplusplus
}
#elif

#endif // BASEDEF_H
