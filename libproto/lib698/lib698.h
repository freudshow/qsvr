#ifndef LIB698_H
#define LIB698_H

#ifdef __cplusplus
extern "C" {
#endif

#include "libutil/basedef.h"
#include "apduType.h"

#define DLT69845_START_CHAR    0x68    //start code of a frame
#define DLT69845_END_CHAR      0x16    //end code of a frame
#define DLT69845_FRM_WAKEUP      0xFE//wake up code
#define DLT69845_FRM_WAKEUP_LEN  4//wake up code's length
#define DLT69845_FRM_SCAMBLE     0x33//scambling code

#pragma pack(push)
#pragma pack(1)

typedef union {//little endian
    u8 u8b;//convenient to set value to 0
    struct {
        u8  low     :4;
        u8  high    :4;
    }bcd;
}comBCD_u;

/****************************
 * frame's length definition
 ****************************/
typedef union {
    u16 u16b;//convenient to set value to 0
    struct {//only for little endian frame!
        u16 len	:14;//length
        u16 rev	:2;//reserve
    } len;
} frmLen_u;

/*****************************
 * control code definition
 ****************************/

typedef union {//control code
    u8 u8b;//convenient to set value to 0
    struct {//only for little endian mathine!
#define DLT69845_FUNC_LINK_MANAGE     1//link managment, including "logon", "heart beat" and "logout".
#define DLT69845_FUNC_LINK_UERDATA    3//apdu managment and data exchange.
        u8 func             :3;//function code
#define DLT69845_SCAMBLE_WITH        1
#define DLT69845_SCAMBLE_NOT_WITH    0
        u8 sc               :1;//Scrambling Code flag. 1-apdu add 0x33 by each byte; 0-apdu don't add 0x33 by each byte.
                        //response's Scrambling format mast be the same as request's/report's Scrambling format.
        u8 rev              :1;//reserve
#define DLT69845_DIV_PIECE           1//divide frame flag. a piece of a frame
#define DLT69845_DIV_COMPLETE        0//divide frame flag. complete frame
        u8 divS             :1;//divide frame flag. 0-this is a complete frame; 1-this is a piece of a frame.
#define DLT69845_PRM_BY_CLT          1//promotion flag, frame promoted by client
#define DLT69845_PRM_BY_SRV          0//promotion flag, frame promoted by server
        u8 prm              :1;//promote flag
#define DLT69845_DIR_SEND_BY_CLT     0//direction flag, frame sent by client
#define DLT69845_DIR_SEND_BY_SRV     1//direction flag, frame sent by server
        u8 dir              :1;//direction flag
    } ctl;
    struct {
        u8 rev              :6;//reserve
#define DLT69845_DIR_PRM_CLT_RESPONSE 0//client's response to server's report
#define DLT69845_DIR_PRM_CLT_REQUEST  1//client's request
#define DLT69845_DIR_PRM_SRV_REPORT   2//server's report
#define DLT69845_DIR_PRM_SRV_RESPONSE 3//server's response for client's request
        u8 dpAssem          :2;//direction and promotion assembly
    } dirPrm;
} ctl_u;

/*****************************************
 * server address's length code definition
 *****************************************/
#define DLT69845_SA_COVER_CODE   0x0F//odd number of a single server address's complement
#define DLT69845_SA_MAX_LEN      16//max length of server address
#define DLT69845_SA_LEN(saLen)   ((saLen)+1)

typedef union {//little endian
    u8 u8b;//convenient to set value to 0
    struct {//only for little endian mathine!
        u8 saLen            :4;//server address's length
        u8 logicAddr        :2;//logic addr
#define DLT69845_SA_TYPE_SINGLE      0
#define DLT69845_SA_TYPE_WILDCARD    1
#define DLT69845_SA_TYPE_GROUP       2
#define DLT69845_SA_TYPE_BROADCAST   3
        u8 saType           :2;//server address's type, 0-single, 1-wildcard, 2-group, 3-broadcast.
    } sa;
} sa_u;

typedef struct {
    sa_u       saLen;
    comBCD_u*  sa;//compression BCD
} srvAddr_s;


/**************************
 * frame header definition
 **************************/

#define DLT69845_HEAD_LEN_BEFORE_SA  (sizeof(u8)+sizeof(u16)+sizeof(ctl_u)+sizeof(sa_u))
#define DLT69845_HEAD_LEN_EXCEPT_SA  (DLT69845_HEAD_LEN_BEFORE_SA+sizeof(u8)+sizeof(u16))
#define DLT69845_HEAD_LEN(saLen)    (DLT69845_HEAD_LEN_EXCEPT_SA+DLT69845_SA_LEN(saLen))

typedef struct frameHead {
    u8          startChar;//start code
    frmLen_u    frmLen;//frame length,
    ctl_u       ctlChar;//ctrol code
    srvAddr_s   sa;//server address
    u8          ca;//client address
    u16         headChk;//crc of head
    u16          headLen;//length of frame's head, including startChar
    u16          apduLen;//length of apdu
}frmHead_s;


typedef struct {
    frmHead_s   head;
    apdu_t      apdu;
    u16         frmChk;
    u8          endChar;
} frm698_s;
typedef frm698_s* frm698_p;

#pragma pack(pop)


extern boolean checkFrame(u8* buf, u16* bufSize, frmHead_s* pFrmhead);
extern u8 decodeFrame(u8* buf, u16 bufSize, frm698_p pFrm);

#ifdef __cplusplus
}
#endif

#endif // LIB698_H
