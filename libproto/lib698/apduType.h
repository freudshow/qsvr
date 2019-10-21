#ifndef APDUTYPE_H
#define APDUTYPE_H

#include "baseDataType.h"

#ifdef __cplusplus
extern "C" {
#endif



/*************************************************
 * link-apdu start
 *************************************************/

#define DLT69845_link_request_type_login        0
#define DLT69845_link_request_type_heartBeat    1
#define DLT69845_link_request_type_exit         2
typedef enumerated_t link_request_type_t;

typedef struct link_request_s {
    piid_acd_t              piidacd;
    link_request_type_t     type;       //请求类型,
    long_unsigned_t         heart;      //心跳周期单位: 秒
    date_time_t             requestTime;
} link_request_t;


typedef struct link_result_s {
    u8 res          :3;     //结果. 0-成功, 1-地址重复, 2-非法设备, 3-容量不足, 其它值-保留
    u8 rsv          :4;     //保留
    u8 clkTrusty    :1;     //时钟可信标志. 0-不可信; 1-可信
} link_result_t;

typedef struct link_response_s {
    piid_t piid;
    link_result_t result;
    long_unsigned_t heart;
    date_time_t    requestTime;
    date_time_t    receiveTime;
    date_time_t    responseTime;
} link_response_t;


typedef union apduStruct {//todo: to be completed
    u8   foo;
} apdu_t;



/*
 * LINK-APDU ::= CHOICE {
 *      linkRequest [1] LINK-Request,	    --预连接请求
 *      linkResponse [129] LINK-Response	--预连接响应
 * }
 */
typedef struct link_apdu_s {
    enum {
        e_link_request_type = 1,
        e_link_response_type = 129
    } e;
    union {
        link_request_t link_request;        //预连接请求
        link_response_t link_response;      //预连接响应
    } l;
} link_apdu_t;
typedef link_apdu_t* link_apdu_p;

/*************************************************
 * link-apdu end
 *************************************************/

extern boolean decodeLinkApdu(u8* apdu, u32 apduLen, link_apdu_p pLinkApdu);

#ifdef __cplusplus
}
#endif

#endif // APDUTYPE_H
