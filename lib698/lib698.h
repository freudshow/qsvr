#ifndef LIB698_H
#define LIB698_H

#ifdef __cplusplus
extern "C" {
#endif

#include "basedef.h"

#pragma pack(push)
#pragma pack(1)

typedef union {//frame's length
    u16 u16b;//convenient to set value to 0
    struct {//only for little endian frame!
        u16 len	: 14;//length
        u16 rev	: 2;//reserve
    } len;
} frmLen_u;

typedef union {//control code
    u8 u8b;//convenient to set value to 0
    struct {//only for little endian mathine!
        u8 func		: 3;//function code
        u8 sc		: 1;//扰码标志
        u8 rev		: 1;//保留
        u8 divS		: 1;//dived frame flag
        u8 prm		: 1;//promote flag
        u8 dir		: 1;//direction flag
    } ctl;
} ctl_u;

typedef union {//server address's length word
    u8 u8b;//convenient to set value to 0
    struct {//only for little endian mathine!
        u8 saLen		: 4;//server address's length
        u8 logicAddr	: 2;//logic addr
        u8 saType	: 2;//server address's type, 0 - single, 1 - wildcard, 2 - group，3 - broadcast.
    } sa;
} sa_u;

typedef struct {
    sa_u    saLen;
    u8   sa[16];
} srvAddr_s;

typedef struct frameHead {
    u8          startChar;//start code
    frmLen_u    frmLen;//frame length,
    ctl_u       ctlChar;//ctrol code
    srvAddr_s   sa;//server address
    u8          ca;//client address
    u16         headChk;//crc of head,
}frmHead_s;

typedef union apduStruct {//todo: to be completed
    u8   foo;
} apdu_s;

typedef struct {
    frmHead_s   head;
    apdu_s      apdu;
    u16         frmChk;
    u8          endChar;
} frm698_s;

#pragma pack(pop)
#ifdef __cplusplus
}
#endif

#endif // LIB698_H
