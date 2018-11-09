#ifndef LIBCJ188_H
#define LIBCJ188_H


#ifdef __cplusplus
extern "C" {
#endif

#include "libutil/basedef.h"

//CJ188数据区最小长度: 1开始+1仪表类型+7地址域+1控制码+1长度+1cs+1结束
#define CJ188_MINSIZE   (13)
#define CJ188_MAXSIZE   (256+CJ188_MINSIZE)         //CJ188数据区最大长度

#define CJ188_PREFIX        0xFE    //前导符, 有的厂家的前导符可能不是0xFE, 需小心
#define CJ188_START_CHAR    0x68    //开始符
#define CJ188_END_CHAR      0x16    //结束符



/****错误码定义*****************/
#define CHK_SUC             0           //校验通过, 可以是单纯的头部校验, 单纯的数据区校验, 也可以是报文的全部结构校验通过
#define ERR_RCVD_LOST       -1          //接收报文不完整
#define ERR_LOST_0x68       -2          //报文开头0x68丢失
#define ERR_LOST_0x16       -3          //报文结束0x16丢失
#define ERR_SUM_ERROR       -4          //总加和校验失败
#define ERR_METERTYPE       -5          //仪表类型不支持
#define ERR_MONTH           -6          //错误历史月

#define ERR_HEAD            -7          //报文的链路层错误
#define ERR_DATA_FMT        -8          //报文的数据层格式错误
#define ERR_CTL_FUNC_NDEF   -9          //控制码的功能码域未定义
#define ERR_DI_NDEF         -10         //数据标识未定义

/****表计类型定义*****************/
#define T_WATER_COOL    0x10        //冷水水表
#define T_WATER_HEAT    0x11        //生活热水水表
#define T_WATER_DRINK   0x12        //直饮水水表
#define T_WATER_MIDDLE  0x13        //中水水表
#define T_HOT_HEAT      0x20        //热量表(计热量)
#define T_HOT_COOL      0x21        //热量表(计冷量)
#define T_GAS           0x30        //燃气表
#define T_METER         0x40        //电度表


#pragma pack(push)
#pragma pack(1)

/**************************CJ188帧结构********************************/

typedef union {         //cj188控制码, 用于报文的直接拷贝, 故不能改变顺序
    u8 u8b;             //方便将结构体清零
    struct {
#define CTL_RSV                 0x00//保留;
#define CTL_R_DATA              0x01//读数据;加密传输
#define CTL_W_DATA              0x04//写数据;加密传输
#define CTL_R_KEYVER            0x09//读密钥版本号;明码传输
#define CTL_R_ADDR              0x03//读地址(表号);单机通讯,明码传输
#define CTL_W_ADDR              0x15//写地址(表号);调试阶段明码传输,收到出厂启用命令后不再响应
#define CTL_W_ELEC_SYNC_DATA        0x16//写机电同步数(置表底数);调试阶段明码传输,收到出厂启用命令后不再响应
#define CTL_VEDOR_DEF(code)     (0x20+(code))//厂商自定义
        u8 func     :6;//功能码, 定义见上
#define CJ188_ASW_VALID     0//通信正常
#define CJ188_ASW_INVALID   1//通信不正常,
        u8 valid    :1;//通信是否正常, 0-正常, 1-不正常, 从cj188协议的8.1.4节中可分析出: 这个位用于判断从站的应答帧是否正常
#define CJ188_DOWNFRM       0//主站发出的控制帧
#define CJ188_UPFRM         1//从站发出的应答帧
        u8 dir      :1;//方向, 0-主站发出的控制帧, 1-从站发出的应答帧
    } ctl;
} ctl188_u;

typedef struct {
    u8 b[7];
} addrB_s;

typedef struct {
    u8 b[5];
    u16 vendor;
} addrV_s;

typedef union {
    addrB_s b7;
    addrV_s laddr;
} logicAddr_s;

typedef struct {            //cj188报文头
    u8 start;               //报文开始字, 0x68
    u8 type;                //仪表类型
    logicAddr_s addr;       //表地址
    ctl188_u ctl;           //控制码
    u8 len;                 //数据域长度
} cj188Head_s;
typedef cj188Head_s* cj188Head_p;


typedef struct {                                //cj188数据区
#define    DI_MEASURE                   0x901F//读计量数据
#define    DI_HISTORY_0                 0xD120//历史计量数据1上 1月结算日累积流量或热量
#define    DI_HISTORY_1                 0xD121//历史计量数据2上 2月结算日累积流量或热量
#define    DI_HISTORY_2                 0xD122//历史计量数据3上 3月结算日累积流量或热量
#define    DI_HISTORY_3                 0xD123//历史计量数据4上 4月结算日累积流量或热量
#define    DI_HISTORY_4                 0xD124//历史计量数据5上 5月结算日累积流量或热量
#define    DI_HISTORY_5                 0xD125//历史计量数据6上 6月结算日累积流量或热量
#define    DI_HISTORY_6                 0xD126//历史计量数据7上 7月结算日累积流量或热量
#define    DI_HISTORY_7                 0xD127//历史计量数据8上 8月结算日累积流量或热量
#define    DI_HISTORY_8                 0xD128//历史计量数据9上 9月结算日累积流量或热量
#define    DI_HISTORY_9                 0xD129//历史计量数据10上 10月结算日累积流量或热量
#define    DI_HISTORY_A                 0xD12A//历史计量数据11上 11月结算日累积流量或热量
#define    DI_HISTORY_B                 0xD13B//历史计量数据12上 12月结算日累积流量或热量
#define    DI_PRICE_TBL                 0x8102//读价格表
#define    DI_R_SETTLE_DATE             0x8103//读结算日
#define    DI_R_READMETER_DATE          0x8104//读抄表日
#define    DI_R_CASH                    0x8105//读购人金额
#define    DI_R_KEYVER                  0x8106//读密钥版本号
#define    DI_R_ADDR                    0x810A//读地址
#define    DI_W_PRICE_TBL               0xA010//写价格表
#define    DI_W_SETTLE_DATE             0xA011//写结算日
#define    DI_W_READMETER_DATE          0xA012//写抄表日
#define    DI_W_CASH                    0xA013//写购入金额
#define    DI_W_NEWKEY                  0xA014//写新密钥
#define    DI_W_STD_TIME                0xA015//写标准时间
#define    DI_W_VALVE_CTL               0xA017//写阀门控制
#define    DI_W_VEDOR_ENABLE            0xA019//出厂启用, 仪表已出厂, 可正常使用
#define    DI_W_ADDR                    0xA018//写地址, 返回用新地址, 不能用在网络上, 调试阶段明码传物, 收到出厂启用命令后不再响应
#define    DI_W_ELEC_SYNC_DATA          0xA016//写机电同步数据
    u16 DI;                     //数据项标识
    u8 seq;                     //帧序号
    u8 data[CJ188_MAXSIZE];     //数据内容
} cj188Data_s;
typedef cj188Data_s* cj188Data_p;

typedef struct {            //cj188报文结构, 用于报文的直接拷贝, 故不能改变顺序
    cj188Head_s head;       //报文头
    cj188Data_s data;       //报文数据内容
    u8 cs;                  //报文校验和
    u8 end;                 //报文结束符, 0x16
} cj188_s;
typedef cj188_s* cj188_p;
/*******************************************************************************/


/******************应用层基本类型定义************************/

/****CJ188单位定义*****************/
#define    UNIT_Wh          0x02
#define    UNIT_kWh         0x05
#define    UNIT_MWh         0x08
#define    UNIT_MWhX100     0x0A
#define    UNIT_J           0x01
#define    UNIT_kJ          0x0B
#define    UNIT_MJ          0x0E
#define    UNIT_GJ          0x11
#define    UNIT_GJX100      0x13
#define    UNIT_W           0x14
#define    UNIT_kW          0x17
#define    UNIT_MW          0x1A
#define    UNIT_L           0x29
#define    UNIT_M3          0x2C//stere
#define    UNIT_Lh          0x32
#define    UNIT_M3h         0x35

typedef struct {
    u8 c;
    u8 u[10];
} unitStr_s;

/****CJ188单位定义end*****************/

typedef u8 unit_t;  //单位类型
typedef u16 di_t;   //数据标识

typedef union {//状态ST
    u16 u16b;
    struct {
#define ST_VALVE_OPEN       0x00//阀门开
#define ST_VALVE_CLOSE      0x01//阀门关
#define ST_VALVE_INVALID    0x03//阀门异常
        u16 vST     :2;//阀门状态
#define VOL_UNDER   1//电池欠压
#define VOL_VALID   0//电池正常
        u16 vol     :1;//电池是否欠压
        u16 rsv     :2;//保留
        u16 vendor  :11;//厂商定义
    } st;
} state_s;

typedef struct {//4字节的物理量, 无单位
    u8 b[4];
} b4_s;

typedef struct {//3字节的物理量, 无单位
    u8 b[3];
} b3_s;

typedef struct {//3字节的物理量, 无单位
    u8 frac;
    u8 intL;
    u8 intH;
} b3BCD_s;

typedef union {//3字节的物理量, 无单位
    b3_s b;
    b3BCD_s bcd;
} bcd3_u;

typedef struct {//4字节的物理量, 4字节数据+1字节单位
    u8 b[4];
    unit_t u;
} b4unit_s;

typedef struct {//3字节的物理量, 3字节数据+1字节单位
    u8 b[3];
    unit_t u;
} byte3_s;

typedef b4unit_s hotPower_s;//热功率XXXXXX.XX
typedef b4unit_s curHot_s;//当前热量XXXXXX.XX
typedef b4unit_s flowRate_s;//流速(协议里写作流量)XXXX.XXXX
typedef b4unit_s curAccumFlow_s;//当前累积流量XXXXXX.XX
typedef b4unit_s setHot_s;//结算日热量XXXXXX.XX
typedef b4unit_s setAccumFlow_s;//结算日累积流量XXXXXX.XX
typedef b4_s money_s;//金额XXXXXX.XX, 无单位, 默认单位元

typedef b3_s workTime_s;//累积工作时间XXXXXX, 无单位, 默认单位h
typedef b3_s waterTemp_s;//水温XXXX.XX, 无单位, 默认单位℃
typedef b3_s price_s;//价格XXXX.XX, 无单位, 默认单位元/单位用量
typedef b3_s dosage_s;//用量XXXXXX, 无单位, 默认单位(m³)

typedef u8 setDate_s;//结算日
typedef u8 readDate_s;//抄表日
typedef u8 seqNo_s;//序列号
typedef u8 verNo_s;//版本号
typedef u8 buyNo_s;//购买序号
typedef struct {//密钥
    u8 b[8];
} key_s;

#define OPEN_VALVE 0x55//开阀控制操作
#define CLOSE_VALVE 0x99//关阀控制操作

typedef struct {//实时时间YYYYMMDDhhmmss
    u8 year[2];
    u8 month;
    u8 day;
    u8 hour;
    u8 min;
    u8 sec;
} realTime_s;


/****************主站读取数据, 从站应答数据类型***************/

/*
 * 数据标识 DI，序号 SER ，当前累积
 * 流量，结算日累积流量，实时时间，
 * 状态 ST
 */
typedef struct {//仪表类型T = 10H~19H, 读计量数据返回格式
    di_t di;                    //数据标识 DI
    u8 seq;                     //序号 SER
    curAccumFlow_s curFlow;     //当前累积流量
    setAccumFlow_s setFlow;     //结算日累积流量
    realTime_s tm;              //实时时间
    state_s st;                 //状态 ST
} readData1019_s;

/*
 * 数据标识 DI，序号 SER ，结算日热
 * 量，当前热量，热功率，流量，累积
 * 流量，供水温度，回水温度，累积工
 * 作时间，实时时间，状态ST
 */
typedef struct {//仪表类型T = 20H~29H, 读计量数据返回格式
    di_t di;                    //数据标识 DI
    u8 seq;                     //序号 SER
    setHot_s sethot;            //结算日热量
    curHot_s curhot;            //当前热量
    hotPower_s hotpower;        //热功率
    flowRate_s flowrate;        //流速
    curAccumFlow_s accumflow;   //累积流量
    waterTemp_s intemp;         //供水温度
    waterTemp_s outtemp;            //回水温度
    workTime_s worktime;        //累积工作时间
    realTime_s tm;              //实时时间
    state_s st;                 //状态 ST
} readData2029_s;


/*
 * 历史数据
 * 上1月至上12月历史数据
 * 格式相同
 */
typedef struct {                //历史数据仪表类型T = 10H~19H, T = 30H~49H
    di_t di;                    //数据标识 DI
    u8 seq;                     //序号 SER
    setAccumFlow_s setFlow;     //结算日累积流量
} hisData10193049_s;


/*
 * 历史数据
 * 上1月至上12月历史数据
 * 格式相同
 */
typedef struct {                //历史数据仪表类型T =  20H~29H
    di_t di;                    //数据标识 DI
    u8 seq;                     //序号 SER
    setHot_s sethot;            //结算日热量
} hisData2029_s;

typedef struct {                //价格表
    di_t di;                    //数据标识 DI
    u8 seq;                     //序号 SER
    price_s price1;             //价格1
    dosage_s dosage1;           //用量1
    price_s price2;             //价格2
    dosage_s dosage2;           //用量2
    price_s price3;             //价格3
} priceTbl_s;

typedef struct {                //抄表日
    di_t di;                    //数据标识 DI
    u8 seq;                     //序号 SER
    readDate_s date;            //超标日
} rDate_s;

typedef struct {                //结算日
    di_t di;                    //数据标识 DI
    u8 seq;                     //序号 SER
    setDate_s date;             //结算日
} sDate_s;

/*
 * 数据标识 DI，序号 SER ，本次购买
 * 序号，本次购入金额，累计购人金
 * 额，剩余金额，状态ST
 */
typedef struct {                //购入金额
    di_t di;                    //数据标识 DI
    u8 seq;                     //序号 SER
    buyNo_s buyno;              //本次购买序号
    money_s thisMny;            //本次购入金额
    money_s accMny;             //累计购入金额
    money_s leftMny;            //剩余金额
    state_s st;                 //状态 ST
} buyMoney_s;

typedef struct {                //密钥版本号
    di_t di;                    //数据标识 DI
    u8 seq;                     //序号 SER
    verNo_s verno;              //版本号
} keyVerNo_s;

typedef struct {                //从站异常应答
    di_t di;                    //数据标识 DI
    state_s st;                 //状态ST
} slaveInvalid_s;




/****************主站写入数据类型, 从站应答数据类型end***************/


/***********************************基本类型定义end********************************/

/****************************************************主站读取数据, 从站应答数据类型***************************************/

/*
 * 由于主站在读数据时, 数据结构是固定的, 故没有必要实现其类型
 */

/***********************************水表类型定义***********************************/

/*
 * 水表数据类型, 包括读取, 写入的数据类型.
 * 使用结构体中的enum来读取或设定具体的数据
 * 类型, 然后根据enum来使用对应的类型.
 */
typedef struct {
    enum {
        e_readDataW=1,//防止结构初始化时, e被初始化为0, 引起外部判断的混淆
        e_hisDataW,
        e_priceTblW,
        e_rdateW,
        e_sdateW,
        e_buyMoneyW,
        e_keyverW,
        e_slaveInvalidW
    } e;
    union {
        readData1019_s rdata;//读取计量数据
        hisData10193049_s hdata;//历史数据
        priceTbl_s priceTbl;//价格表
        rDate_s rdate;//抄表日
        sDate_s sdate;//结算日
        buyMoney_s buymny;//购入金额
        keyVerNo_s keyver;//密钥版本号
        slaveInvalid_s invalid;//从站u异常应答
    } u;
} waterData_s;
typedef waterData_s* waterData_p;

/***********************************水表类型定义end********************************/


/***********************************热表类型定义***********************************/

/*
 * 热表数据类型, 包括读取, 写入的数据类型.
 * 使用结构体中的enum来读取或设定具体的数据
 * 类型, 然后根据enum来使用对应的类型.
 */
typedef struct {
    enum {
        e_readDataH=1,//防止结构初始化时, e被初始化为0, 引起外部判断的混淆
        e_hisDataH,
        e_priceTblH,
        e_rdateH,
        e_sdateH,
        e_buyMoneyH,
        e_keyverH,
        e_slaveInvalidH
    } e;
    union {
        readData2029_s rdata;//读取计量数据
        hisData2029_s hdata;//历史数据
        priceTbl_s priceTbl;//价格表
        rDate_s rdate;//抄表日
        sDate_s sdate;//结算日
        buyMoney_s buymny;//购入金额
        keyVerNo_s keyver;//密钥版本号
        slaveInvalid_s invalid;//从站u异常应答
    } u;
} hotData_s;
typedef hotData_s* hotData_p;

/***********************************热表类型定义end********************************/


/***********************************燃气表类型定义***********************************/

/*
 * 燃气表数据类型, 包括读取, 写入的数据类型.
 * 使用结构体中的enum来读取或设定具体的数据
 * 类型, 然后根据enum来使用对应的类型.
 */
typedef struct {
    enum {
        e_readDataG=1,//防止结构初始化时, e被初始化为0, 引起外部判断的混淆
        e_hisDataG,
        e_priceTblG,
        e_rdateG,
        e_sdateG,
        e_buyMoneyG,
        e_keyverG,
        e_slaveInvalidG
    } e;
    union {
        readData1019_s rdata;//读取计量数据
        hisData10193049_s hdata;//历史数据
        priceTbl_s priceTbl;//价格表
        rDate_s rdate;//抄表日
        sDate_s sdate;//结算日
        buyMoney_s buymny;//购入金额
        keyVerNo_s keyver;//密钥版本号
        slaveInvalid_s invalid;//从站u异常应答
    } u;
} gasData_s;
typedef gasData_s* gasData_p;

/***********************************燃气表类型定义end********************************/

typedef struct {
    enum {
        e_waterData=1,//防止结构初始化时, e被初始化为0, 引起外部判断的混淆
        e_hotData,
        e_gasData,
    } e;
    union {
        waterData_s wData;
        hotData_s   hData;
        gasData_s   gData;
    } u;
} meterData_s;
typedef meterData_s* meterData_p;

/****************************************************主站读取数据, 从站应答数据类型end***************************************/

#pragma pack(pop)

extern s16 cj188unpack(cj188_p pCj188, u8* rcvbuff, u16 rcvlen);
extern s16 cj188_parseData(cj188_p pCj188, meterData_p pData);
extern s8 cj188_parseWaterData(cj188_p pCj188, waterData_p pdata);
extern s8 getWaterData(cj188_p pCj188, waterData_p pdata);
extern s8 cj188_parseHotData(cj188_p pCj188, hotData_p pdata);
extern s8 getHotData(cj188_p pCj188, hotData_p pdata);
extern s8 cj188_parseGasData(cj188_p pCj188, gasData_p pdata);
extern s8 getGasData(cj188_p pCj188, gasData_p pdata);
extern void getUnitStr(u8 c, char* unit);
extern u8 checkSum(u8* buf, u16 len);
extern s16 firstChk(u8* buf, u16 bufSize);
extern float fixP2floatP(u8* buf, u16 len, u8 pos, u8 order, u8 sign);
extern s64 fixP2s64(u8* buf, u16 len, u8 pos, u8 order, u8 sign);

extern s8 cj188compose(cj188_p pCj188, u8* buf, u16* bufSize, u8 preCnt);
extern s8 cj188ReadData(cj188_p pCj188, u8* buf, u16* bufSize, u8 preCnt);
extern s8 cj188UpReadData(cj188_p pCj188, meterData_p pData, u8* buf, u16* bufSize, u8 preCnt);


#ifdef __cplusplus
}
#endif

#endif // LIBCJ188_H
