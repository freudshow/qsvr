#ifndef LIB698_H
#define LIB698_H

#ifdef __cplusplus
extern "C" {
#endif

#include "basedef.h"

#define FRM_PREFIX      0x68//start code of a frame
#define FRM_SUFFIX      0x16//end code of a frame
#define FRM_WAKEUP      0xFE//wake up code
#define FRM_WAKEUP_LEN  4//wake up code's length
#define FRM_SCAMBLE     0x33//scambling code

#pragma pack(push)
#pragma pack(1)


//698.45扩展数据类型-------------------------------------
typedef unsigned char   MAC_698;//数据安全MAC
typedef unsigned char   RN_698;	//random number
typedef unsigned short  OI_698;	//object's identifier

/****************************
 * frame's length definition
 ****************************/
typedef union {
    u16 u16b;//convenient to set value to 0
    struct {//only for little endian frame!
        u16 len	: 14;//length
        u16 rev	: 2;//reserve
    } len;
} frmLen_u;

/*****************************
 * control code definition
 ****************************/
#define DIR_SEND_BY_CLT     0//direction flag, frame sent by client
#define DIR_SEND_BY_SRV     1//direction flag, frame sent by server

#define PRM_BY_CLT          1//promotion flag, frame promoted by client
#define PRM_BY_SRV          0//promotion flag, frame promoted by server

#define DIR_PRM_CLT_RESPONSE 0//client's response to server's report
#define DIR_PRM_CLT_REQUEST  1//client's request
#define DIR_PRM_SRV_REPORT   2//server's report
#define DIR_PRM_SRV_RESPONSE 3//server's response for client's request

#define FUNC_LINK_MANAGE     1//link managment, including "logon", "heart beat" and "logout".
#define FUNC_LINK_UERDATA    3//apdu managment and data exchange.

typedef union {//control code
    u8 u8b;//convenient to set value to 0
    struct {//only for little endian mathine!
        u8 func		: 3;//function code
        u8 sc		: 1;//Scrambling Code flag. 1-apdu add 0x33 by each byte; 0-apdu don't add 0x33 by each byte.
                        //response's Scrambling format mast be the same as request's/report's Scrambling format.
        u8 rev		: 1;//reserve
        u8 divS		: 1;//divide frame flag. 0-this is a complete frame; 1-this is a piece of a frame.
        u8 prm		: 1;//promote flag
        u8 dir		: 1;//direction flag
    } ctl;
    struct {
        u8 rev      :6;//reserve
        u8 dpAssem  :2;//direction and promotion assembly
    } dirPrm;
} ctl_u;

/*****************************************
 * server address's length code definition
 *****************************************/
#define SA_COVER_CODE   0x0F//odd number of a single server address's complement
#define SA_MAX_LEN      16//max length of server address
#define SA_LEN(saLen)   ((saLen)+1)

typedef union {//little endian
    u8 u8b;//convenient to set value to 0
    struct {//only for little endian mathine!
        u8 saLen		: 4;//server address's length
        u8 logicAddr	: 2;//logic addr
        u8 saType       : 2;//server address's type, 0-single, 1-wildcard, 2-group，3-broadcast.
    } sa;
} sa_u;

typedef union {//little endian
    u8 u8b;//convenient to set value to 0
    struct {
        u8  low     :4;
        u8  high    :4;
    }bcd;
}comBCD_u;

typedef struct {
    sa_u        saLen;
    comBCD_u   sa[SA_MAX_LEN];//compression BCD
} srvAddr_s;


/**************************
 * frame header definition
 **************************/

#define HEAD_LEN_BEFORE_SA  (sizeof(u8)+sizeof(u16)+sizeof(ctl_u)+sizeof(sa_u))
#define HEAD_LEN_EXCEPT_SA  (HEAD_LEN_BEFORE_SA+sizeof(u8)+sizeof(u16))
#define HEAD_LEN(saLen)    (HEAD_LEN_EXCEPT_SA+SA_LEN(saLen))

typedef struct frameHead {
    u8          startChar;//start code
    frmLen_u    frmLen;//frame length,
    ctl_u       ctlChar;//ctrol code
    srvAddr_s   sa;//server address
    u8          ca;//client address
    u16         headChk;//crc of head
    u8          headLen;//length of frame's head, including startChar
}frmHead_s;

typedef union apduStruct {//todo: to be completed
    u8   foo;
} apdu_s;

typedef struct {
    frmHead_s   head;
    apdu_s*     apdu;
    u16         frmChk;
    u8          endChar;
} frm698_s;

#pragma pack(pop)

extern u8 processFrame(u8* buf, u16 bufSize);

#ifdef __cplusplus
}
#endif

#endif // LIB698_H
