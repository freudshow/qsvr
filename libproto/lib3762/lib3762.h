#ifndef LIB3762_H
#define LIB3762_H

#ifdef __cplusplus
extern "C" {
#endif

#include "libutil/basedef.h"

#pragma pack(push)
#pragma pack(1)


typedef struct ctl3762{
#define CTL_3762_CENT_PLC   1       //集中式路由载波通信
#define CTL_3762_DIST_PLC   2       //分布式路由载波通信
#define CTL_3762_WIRELESS   10      //采用微功率无线组网的通信
#define CTL_3762_ETH        20      //以太网通信
        u8 way     : 6;              //通信方式
#define CTL_3762_VALID       0       //从站正确应答
#define CTL_3762_INVALID     1       //从站异常应答
        u8 prm    : 1;               //启动标志位, 0: 报文来自启动站, 1: 报文来自从动站
#define CTL_3762_DOWN        0       //集中器发出的下行报文
#define CTL_3762_UP          1       //通信模块发出的上行报文
        u8 dir    : 1;               //传输方向, 0: 集中器发出的下行报文, 1: 通信模块发出的上行报文
} ctl3762_s;

typedef struct infoDomain3762down{
    struct {
        u8 routeId      :1;//路由标识, 0 表示通信模块带路由或工作在路由模式,1 表示通信模块不带路由或工作在旁路模式。
        u8 attachId     :1;//附属节点标识, 指载波从节点附属节点标识,0 表示无附加节点,1 表示有附加节点。
        u8 commModId    :1;//通信模块标识, 0 表示对集中器的通信模块操作,1 表示对载波表的通信模块操作。
        u8 conflictChk  :1;//冲突检测,0 表示不进行冲突检测,1 表示要进行冲突检测。
        u8 relayLevel   :4;//中继级别,取值范围 0~15,0 表示无中继。
    }route;
    struct {
        u8 channelId    :4;//信道标识, 取值0～15，0表示不分信道、1～15依次表示第1～15信道。
        u8 errCodeId    :4;//纠错编码标识, 取值范围0～15，0表示信道未编码，1表示RS编码，2～15保留。
    }channel;
    u8 anwLen;             //预计应答字节数
    struct {
        u16 id          :1;//速率单位标识, 0 表示 bps,1 表示 kbps。
        u16 rate       :15;//通信速率, 表示通信波特率,BIN 格式,0 表示默认通信速率。
    }baud;                 //通信速率
    u8 resv;               //预留
} infoDomain3762down_s;

typedef struct infoDomain3762up{
    struct {
        u8 routeId      :1;//路由标识,D0=0 表示通信模块带路由或工作在路由模式,D0=1 表示通信模块不带路由或工作在旁路模式。
        u8 reserv0      :1;//预留,
        u8 commModId    :1;//通信模块标识,0 表示对集中器的通信模块操作,1 表示对载波表的通信模块操作。
        u8 reserv1      :1;//预留
        u8 relayLevel   :4;//中继级别,取值范围 0~15,0 表示无中继
    }route;
    struct {
        u8 reserv       :4;//预留
        u8 channelId    :4;//信道标识,取值 0~15,0 表示不分信道、1~15 依次表示第 1~15 信道
    }channel;
    struct {
        u8 phaseId      :4;//实测相线标识,实测从节点逻辑主信道所在电源相别,0 为不确定,1~3 依次表示相别为第 1 相、第 2 相、第 3 相。
        u8 meterRouteId :4;//电表通道特征,描述目的节点电表通道的特征,取值范围 0~15,0 保留,1 为载波物理信道
                           //为单相供电,逻辑信道为单信道;2 为载波物理信道为单相供电,逻辑信道为两信道;3 为载波物理信
                           //道为单相供电,逻辑信道为三信道;4 为载波物理信道为三相供电,逻辑信道为三信道。
    }meter;
    struct {
        u8 cmdQua       :4;//末级命令信号品质,
        u8 aswQua       :4;//末级应答信号品质,
    }quality;              //分为 15 级,取值范围 0~15,0 表示无信号品质,1 表示最低品质。
    u16 resv;               //预留
} infoDomain3762up_s;

typedef struct infoDomain3762{
    enum {
       downFrm = 0,
       upFrm
    }e;
    union {
        infoDomain3762down_s down;
        infoDomain3762up_s up;
    };
} infoDomain3762_s;

typedef struct logicAddr {
    bcd_s bcd[6];
} logicAddr_s;

/*
 * 当信息域的“通信模块标识”为 0 时,无地址域 A;
 * 当信息域的“通信模块标识”为 1 时,载波主节点下行时,源地址 A1 是指载波主节点的 MAC
 * 地址,中继地址 A2 和目的地址 A3 是指载波从节点的 MAC 地址;载波从节点上行时,源地址 A1 是指
 * 载波从节点的 MAC 地址,无中继地址 A2,目的地址 A3 是指载波主节点的 MAC 地址。
 * 当为广播命令时,目的地址 A3 为广播地址 999999999999H。
 */
typedef struct addrDomain3762{
    u8 relayLevel;              //中继级别, 最多15级
    logicAddr_s src;            //源地址
    logicAddr_s relay[15];      //中继地址, 个数等于中继级别
    logicAddr_s dest;           //目的地址
} addrDomain3762_s;

typedef struct infoClass3762 {
    u8 dt1;
    u8 dt2;
}infoClass3762_s;



/******************************* AFN=00 确认/否认 ***********************************/
typedef struct AFN00_F1{
    u8 CommandStatus;		//命令状态
    u8 ChannelStatus[31];	//信道状态
    u16 WaitingTime;			//等待时间
}AFN00_F1_s;	//确认

typedef struct
{
    u8 ErrStatus;		//错误状态字
}AFN00_F2;	//否认















typedef struct appData3762 {
    u8 afn;
    infoClass3762_s dt;
} appData3762_s;

typedef struct {
    infoDomain3762_s infoDomain;
    addrDomain3762_s addr;
} userData3762_s;

typedef struct {
    u8 start;
    u16 len;
    ctl3762_s ctl;
    userData3762_s userData;
    u8 cs;
    u8 end;
} frm3762_s;

#pragma pack(pop)

#ifdef __cplusplus
}
#endif

#endif // LIB3762_H
