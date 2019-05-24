#ifndef LIB3761_H
#define LIB3761_H

#ifdef __cplusplus
extern "C" {
#endif

#include "libutil/basedef.h"
#pragma pack(push)
#pragma pack(1)

#define QGDW13761_START_CHAR    0x68    //开始符
#define QGDW13761_END_CHAR      0x16    //结束符

typedef struct q3761Len {
#define PROTOID_FORBIDDEN   0
#define PROTOID_130_2005    1
#define PROTOID_13761       2
#define PROTOID_RESERVE     3
    u16 protoId     :2;     //协议标识由图 2 中 D0～D1 两位编码表示
                            //a） D0=0、D1=0：为禁用；
                            //b） D0=1、D1=0：为《Q/GDW 130—2005 电力负荷管理系统数据传输规约》使用；
                            //c） D0=0、D1=1：为本协议使用；
                            //d） D0=1、D1=1，为保留。
    u16 len         :14;    //用户数据区的长度
}q3761Len_s;

typedef struct q3761Ctl {
    u8 func     :4;         //功能码
    u8 fcv      :1;         //下行方向, 帧计数有效位FCV
    u8 fcb_acd  :1;         //下行方向, 帧计数位 FCB; 上行方向, 要求访问位 ACD
    u8 prm      :1;         //启动标志位PRM
    u8 dir      :1;         //传输方向位DIR
}q3761Ctl_s;

typedef struct q3761Addr {
    bcd_s   district0;      //行政区划码0
    bcd_s   district1;      //行政区划码1
    u16     tAddr;          //终端地址
    u8      sAddr;          //主站地址和组地址标志
}q3761Addr_s;

typedef struct QGDW13761UserData {
    q3761Ctl_s  ctl;                //控制域
    q3761Addr_s Addr;               //地址域
    //链路用户数据
}QGDW13761UserData_s;

typedef struct QGDW13761struct{
    u8 start0;                      //开始符0
    q3761Len_s len0;                //用户数据区的长度0
    q3761Len_s len1;                //用户数据区的长度1
    u8 start1;                      //开始符1
    QGDW13761UserData_s uData;      //用户数据区
    u8 cs;                          //用户数据区的八位位组的算术和，不考虑进位位
    u8 end;                         //结束符
} QGDW13761_s;

#pragma pack(pop)

#ifdef __cplusplus
}
#endif

#endif // LIB3761_H
