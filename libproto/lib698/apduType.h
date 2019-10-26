#ifndef APDUTYPE_H
#define APDUTYPE_H

#include "baseDataType.h"


#ifdef __cplusplus
extern "C" {
#endif

/*
 *  LINK-APDU ::= CHOICE {
 *      linkRequest [1] LINK-Request,	    --预连接请求
 *      linkResponse [129] LINK-Response	--预连接响应
 *  }
 *
 *  Client-APDU ::= SEQUENCE {
 *      appLayerSvrClient CHOICE {
 *          connectRequest	 	[2]	CONNECT-Request, 	--建立应用链接请求
 *          releaseRequest	 	[3]	RELEASE-Request, 	--断开应用链接请求
 *          getRequest	 	    [5]	GET-Request, 	    --读取请求
 *          setRequest	 	    [6]	SET-Request, 	    --设置请求
 *          actionRequest	 	[7]	ACTION-Request, 	--操作请求
 *          reportResponse	 	[8]	REPORT-Response, 	--上报响应
 *          proxyRequest	 	[9]	PROXY-Request,    	--代理请求
 *          errorResponse       [110] ERROR-Response    --异常响应
 *      },
 *      timeTag TimeTag OPTIONAL --时间标签
 *  }
 *
 *  Server-APDU ::= SEQUENCE {
 *      appLayerSvrServer CHOICE {
 *          connectResponse	 	[130]	CONNECT-Response,       --建立应用链接响应
 *          releaseResponse	 	[131]	RELEASE-Response,       --断开应用连接响应
 *          releaseNotification	[132]	RELEASE-Notification, 	--断开应用连接通知
 *          getResponse	 	    [133]	GET-Response,           --读取响应
 *          setResponse	 	    [134]	SET-Response,           --设置响应
 *          actionResponse	 	[135]	ACTION-Response,        --操作响应
 *          reportNotification	[136]	REPORT-Notification, 	--上报通知
 *          proxyResponse	 	[137]	PROXY-Response,         --代理响应
 *          errorResponse       [238]   ERROR-Response          --异常响应
 *      },
 *      followRpt FollowReport      OPTIONAL,       --跟随上报信息域
 *      timeTag TimeTag             OPTIONAL        --时间标签
 *  }
 *
 */

typedef enum {
    e_link_request_type = 1,

    /****Client-APDU type start****/
    e_connect_request_type = 2,
    e_release_request_type = 3,
    e_get_request_type = 5,
    e_set_request_type = 6,
    e_acton_request_type = 7,
    e_report_request_type = 8,
    e_proxy_request_type = 9,
    e_error_responset_type = 110,
    /****Client-APDU type end****/

    /****Server-APDU type start****/
    e_connect_Response_type = 130,
    e_releaseResponse =131,
    e_releaseNotification =132,
    e_getResponse =133,
    e_setResponse =134,
    e_actionResponse =135,
    e_reportNotification =136,
    e_proxyResponse =137,
    e_errorResponse =238,
    /****Server-APDU type end****/

    e_link_response_type = 129
} apdu_type_e;

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

/*************************************************
 * link-apdu end
 *************************************************/

/*************************************************
 * client-apdu start
 *************************************************/


typedef struct client_apdu_s {
    apdu_type_e e;
    union {

    } client_apdu;
} client_apdu_t;

/*************************************************
 * client-apdu end
 *************************************************/


/*************************************************
 * server-apdu start
 *************************************************/
typedef struct server_apdu_s {
    apdu_type_e e;
    union {

    } server_apdu;
} server_apdu_t;

/*************************************************
 * server-apdu end
 *************************************************/


typedef union apdu_s {//todo: to be completed
    apdu_type_e e;
    link_request_t link_request;        //预连接请求
    link_response_t link_response;      //预连接响应
    client_apdu_t client_apdu;          //客户端应用层数据单元
    server_apdu_t server_apdu;          //服务端应用层数据单元
} apdu_t;
typedef apdu_t* apdu_p;




extern boolean decodeLinkApdu(u8* apdu, u16 apduLen, apdu_p link_apdu_p);

#ifdef __cplusplus
}
#endif

#endif // APDUTYPE_H
