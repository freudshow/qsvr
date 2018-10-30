#ifndef LIBDLT645_H
#define LIBDLT645_H

#include "libutil/basedef.h"

#define DLT645_MAXSIZE  (256)       //DLT645数据区最大长度

//DLT645数据区最小长度: 1开始+6地址域+1个68+1控制码+1长度+1cs+1结束
#define DLT645_MINSIZE  (12)

#define DLT645_MAX_RATE    (63)     //电能表的最大费率数

#define DLT645_PREFIX       0xFE    //前导符, 有的厂家的前导符可能不是0xFE, 需小心
#define DLT645_START_CHAR   0x68    //开始符
#define DLT645_END_CHAR     0x16    //结束符
#define DLT645_BIAS         0x33    //数据域需要加减33处理

/****错误码定义*****************/
#define DLT645_CHK_SUC                0           //校验通过, 可以是单纯的头部校验, 单纯的数据区校验, 也可以是报文的全部结构校验通过
#define DLT645_ERR_RCVD_LOST          -1          //接收报文不完整
#define DLT645_ERR_LOST1_0x68         -2          //报文第1个0x68丢失
#define DLT645_ERR_LOST2_0x68         -3          //报文第2个0x68丢失
#define DLT645_ERR_LOST_0x16          -4          //报文结束0x16丢失
#define DLT645_ERR_SUM_ERROR          -5          //总加和校验失败
#define DLT645_ERR_HEAD               -6          //报文的链路层错误
#define DLT645_ERR_ADDR               -7          //地址域错误
#define DLT645_ERR_DATA_FMT           -8          //报文的数据层格式错误
#define DLT645_ERR_CTL_FUNC_NDEF      -9          //控制码的功能码域未定义
#define DLT645_ERR_DI_NDEF            -10         //数据标识未定义

/*
 * 地址域由 6 个字节构成，每字节 2 位 BCD 码，地址长度可达12位十进制数。每块表具有唯一的
 * 通信地址，且与物理层信道无关。当使用的地址码长度不足 6 字节时，高位用“0”补足。
 * 通信地址999999999999H为广播地址，只针对特殊命令有效，如广播校时和广播冻结等。广播命令
 * 不要求从站应答。
 * 地址域支持缩位寻址，即从若干低位起，剩余高位补AAH作为通配符进行读表操作，从站应答帧的
 * 地址域返回实际通信地址。地址域传输时低字节在前，高字节在后。
 */
#define DLT645_BROADCAST_ADDR        0x99 //6byte, 广播校时和广播冻结等. 广播命令不要求从站应答.
#define DLT645_WILDCARD_ADDR         0xAA //6byte, 通配符, 用于缩位寻址

/*
 * 5.3.3 传输响应
 * 每次通信都是由主站向按信息帧地址域选择的从站发出请求命令帧开始，被请求的从站接收到命
 * 令后作出响应。
 * 收到命令帧后的响应延时 Td： 20ms≤Td≤500ms。
 * 字节之间停顿时间 Tb： Tb≤500ms
 */
#define DLT645_TIMEOUT    500 //抄表最小延迟时间500毫秒



#pragma pack(push)
#pragma pack(1)

typedef struct {    //压缩BCD码格式
    u8 l  :4;       //BCD码低位
    u8 h  :4;       //BCD码高位
} comBCD_s;

typedef union {         //645-07协议控制码结构
    u8 u8b;
    struct {
#define CTL_645_REV         0x00    //保留
#define CTL_645_SEC_AUT     0x03    //安全认证security authentication
#define CTL_645_BROADCAST   0x08    //广播校时
#define CTL_645_RDATA       0x11    //读数据, 也用来透明转发或转发cj188报文
#define CTL_645_RSUCD       0x12    //读后续数据
#define CTL_645_RADDR       0x13    //读通信地址
#define CTL_645_WDATA       0x14    //写数据
#define CTL_645_WADDR       0x15    //写通信地址
#define CTL_645_FRZ         0x16    //冻结命令
#define CTL_645_WBAUD       0x17    //更改通信速率
#define CTL_645_WPASSWD     0x18    //修改密码
#define CTL_645_CLRDEM      0x19    //最大需量清零
#define CTL_645_CLRMETER    0x1A    //电表清零
#define CTL_645_CLRENT      0x1B    //事件清零
#define CTL_645_VALVE       0x1C    //跳闸, 报警, 保电
#define CTL_645_PORTOUT     0x1D    //多功能端子输出
#define CTL_645_FILETRANS   0x1E    //文件传输, 中电联未对文件传输的控制码做出规定, 暂时用0x1E
        u8 func     : 5;//功能码
#define CTL_645_NOSUCCEED   0   //无后续数据帧
#define CTL_645_SUCCEED     1   //有后续数据帧
        u8 succeed  : 1;//有无后继帧, 0: 无后续数据帧, 1: 有后续数据帧
#define CTL_645_VALID       0   //从站正确应答
#define CTL_645_INVALID     1   //从站异常应答
        u8 valid    : 1;//从站应答, 0: 从站正确应答, 1: 从站异常应答
#define CTL_645_DOWN        0   //主站发出的命令帧
#define CTL_645_UP          1   //从站发出的应答帧
        u8 dir      : 1;//传输方向, 0: 主站发出的命令帧, 1: 从站发出的应答帧
    } ctl;
} ctl645_u;

typedef struct {
    u8        start1;
    u8        addr[6];
    u8        start2;
    ctl645_u  ctl;
    u8        len; //为数据域的字节数。读数据时 L≤200，写数据时 L≤50， L=0 表示无数据域
} head645_s;

/***********************************645扩展协议数据标识start******************************************/
//数据标识的4个字节, 先发送DI0, 再依次发送高位数据
#define DLT645_DI_CUR_TIME               0x0400010C //扩展645协议, 读/写系统时间, 7字节
#define DLT645_DI_TRANSMIT               0xE5E50000 //转发全透明报文
#define DLT645_DI_TRANS188               0xE5E50001 //转发CJ/T 188协议转换报文
#define DLT645_DI_MBUS_SLEEP_TIME        0xE5E50101 //采集器M-BUS接口供电休眠时间(BCD格式, 单位min). 00: 工作在持续供电模式; 其他值: 采集器在无通信后相应时间进入休眠模式.
#define DLT645_DI_WIRELESS_DATA_AGING    0xE5E50102 //采集器无线接口数据老化时间(BCD格式, 单位min)
#define DLT645_DI_REC_SAVE_TIME          0xE5E50103 //表计无通信时, 档案保存时间(BCD格式, 单位d)
#define DLT645_DI_LOGIC_ADDRESS          0xE5E50104 //采集器通信地址(只支持点对点设置)
#define DLT645_DI_INIT_PARA              0xE5E50201 //参数初始化
#define DLT645_DI_INIT_DATA              0xE5E50202 //数据初始化
#define DLT645_DI_FILE_TRANS             0xE5E50300 //文件传输
#define DLT645_DI_FILE_NO_DATA           0xE5E50301 //文件传输未收到数据段

/***********************************645扩展协议数据标识end******************************************/

/******************************645-07协议start************************************/
typedef union {    //645-07协议数据标识符结构
    u32 u32b;
    u8  list[4];
    struct {
        u8 di0;//标识符第0字节
        u8 di1;//标识符第1字节
        u8 di2;//标识符第2字节
        u8 di3;//标识符第3字节
    } di;
    struct {
        u8 diA1;  //电能量索引-0x00
        u8 diItem;//电能量内数据项索引
        u8 diRate;//电能量费率索引, 最大0x3F
        u8 diDate;//电能量上n结算日, 最大0x0C
    } diA1;
} di645_u;

typedef struct { //帧序号只有在读取后续帧的报文中才存在
    head645_s head;
    u8      d[256];//dlt645的长度字是1个字节, 所以最多一次能发送255个字节的数据
    u8      cs;
    u8      end;
} DLT645_s;
typedef DLT645_s* DLT645_p;

/*
 * 读写系统时间
 */
typedef struct {
    u8 ss; //秒
    u8 mm; //分
    u8 hh; //时
    u8 DD; //日
    u8 MM; //月
    u8 YY; //年
} dlt645Date_s;


typedef di645_u dlt645downRDDI_s;//读取给定数据项

typedef struct {    //读取负荷曲线
    di645_u di;
    u8      n;
} dlt645downRDCurve_s;

typedef struct {
    di645_u di;
    u8      n;
    dlt645Date_s time;
} dlt645downRDCurvePoint_s;

/*
 * 多字节量的, 最高字节的最高位如果是符号位,
 * 则使用 sign, 否则使用unsign
 */
typedef union {
    struct {      //有符号位
        u8 d  :7; //数据域
        u8 s  :1; //符号位, 0正1负
    } sign;

    comBCD_s d;         //无符号位
} dlt645MSB_u;

/*
 * 电能量类数据类型, 全部是XXXXXX.XX格式,
 * 全部为只读属性.
 * 费率最多63, 再加一个总量, 所以电表应答
 * 一个电能数据块时, 至少需要包含一个总量.
 * 注 1: 组合有功、无功电能最高位是符号位， 0正1负。取值范围： 0.00～799999.99。
 *       所以程序在将BCD码转换为整型值时, 需要看符号位,
 *       由于79999999 = 0x4C4B3FF, 所以要使用4字节的有符号整数类型
 * 注 2: ZZ代表本字节所列数值的任意一个取值， ZZ不能取值为FF。
 * 注 3: 在传输某结算日电能量数据块时，数据块中包含的费率电能以实际设置的费率数为准。
 */
typedef struct {
    comBCD_s Int[3];      //整数部分
    dlt645MSB_u sign;     //整数部分的最高字节
} dlt645ElecEnergy_s;


/*
 * 最大需量及发生时间数据类型
 * 数据格式 XX.XXXX YYMMDDhhmm
 * 共8字节.
 * 只读量.
 * 注 1: 组合无功最大需量的最高位是符号位， 0正1负。取值范围： 0.0000～79.0000。
 * 注 2: 在传输某结算日最大需量及发生时间数据块时，数据块中包含的费率最大需量及发生时间以实际设置
 * 的费率数为准。
 */
typedef struct {
    comBCD_s frac[2];      //小数部分
    dlt645MSB_u Int;       //整数部分
    dlt645Date_s date;     //最大需量的发生时间
} dlt645Demand_s;


/***********变量类数据类型start***********/
/*
 * 只读数据
 * 注 1: 三相三线电表电压A相为Uab， B相为0， C相为Ucb；电流A相为Ia， B相为0， C相为Ic； 功率因数A相为Uab与Ia的夹
 *       角余弦， B相为0， C相为Ucb与Ic的夹角余弦；相角A相为Uab与Ia的夹角， B相为0， C相为Ucb与Ic的夹角。
 * 注 2: 瞬时功率及当前需量最高位表示方向， 0正， 1负，三相三线B相为0。取值范围： 0.0000～79.9999。
 * 注 3: 表内温度最高位0表示零上， 1表示零下。 取值范围： 0.0～799.9。
 * 注 4: 相角测量范围是0～360度。
 * 注 5: 当前有功需量、当前无功需量、当前视在需量是最近一段时间的平均功率。
 * 注 6: 电流最高位表示方向， 0正， 1负，取值范围： 0.000～799.999。功率因数最高位表示方向， 0正， 1负，
 *       取值范围: 0.000～1.000。
 */

typedef struct {
    comBCD_s d[2];
} bcd2unsign_s;

typedef struct {
    comBCD_s    d;
    dlt645MSB_u s;
} bcd2sign_s;

typedef struct {
    comBCD_s d[3];
} bcd3unsign_s;

typedef struct {
    comBCD_s d[2];
    dlt645MSB_u s;
} bcd3sign_s;

typedef bcd2unsign_s voltage_s;       //电压数据类型, 10^(-1)
typedef bcd3sign_s current_s;         //电流数据类型, 10^(-3)
typedef bcd3sign_s power_s;           //功率数据类型, 10^(-4)
typedef bcd2sign_s powerFactor_s;     //功率因数数据类型, 10^(-3)
typedef bcd2unsign_s phaseAngle_s;    //相角数据类型, 10^(-1)
typedef bcd2unsign_s distortion_s;    //波形失真度数据类型, 10^(-2)
typedef bcd2unsign_s harmonic_s;      //电压, 电流的谐波含量, 10^(-2)
typedef bcd2unsign_s freq_s;          //电网频率, 10^(-2)
typedef bcd3sign_s curDemand_s;       //当前需量, 10^(-4)
typedef bcd2sign_s temp645_s;         //表内温度, 10^(-1)
typedef bcd2unsign_s clockV_s;        //时钟电池电压(内部), 10^(-1)
typedef bcd2unsign_s outageRDV_s;     //停电抄表电池电压, 10^(-2)

typedef struct {                    //内部电池工作时间, 1
    comBCD_s d[4];
} batWorkTime_s;

typedef struct {                    //当前阶梯电价, , 10^(-4)
    comBCD_s d[4];
} curPrice_s;


/***********变量类数据类型end***********/

/***********事件类数据类型start***********/

typedef struct {//失压总次数, 总累积时间
    comBCD_s times[3];        //总次数, 次
    comBCD_s accumTime[3];    //总累积时间, min
} lostV_s;

typedef lostV_s lackVTimes_s;//欠压总次数, 总累积时间
typedef lostV_s overVTimes_s;//过压总次数, 总累积时间
typedef lostV_s breakPhaTimes_s;//断相总次数, 总累积时间
typedef lostV_s volRevTimes_s;//电压逆相序总次数，总累计时间
typedef lostV_s curRevTimes_s;//电流逆相序总次数，总累计时间
typedef lostV_s volNotBalanceTimes_s;//电压不平衡总次数，总累计时间




typedef struct {//失压记录
    dlt645Date_s startTime;//开始时间
    dlt645Date_s endTime;//结束时间
    dlt645ElecEnergy_s positiveInc;//失压期间正向有功总电能增量
    dlt645ElecEnergy_s reverseInc;//失压期间反向有功总电能增量
    dlt645ElecEnergy_s reactive1Inc;//失压期间组合无功1总电能增量
    dlt645ElecEnergy_s reactive2Inc;//失压期间组合无功2总电能增量
    dlt645ElecEnergy_s positiveIncA;//失压期间A相正向有功电能增量
    dlt645ElecEnergy_s reverseIncA;//失压期间A相反向有功电能增量
    dlt645ElecEnergy_s reactive1IncA;//失压期间A相组合无功1电能增量
    dlt645ElecEnergy_s reactive2IncA;//失压期间A相组合无功2电能增量
    voltage_s volA;//失压时刻A相电压
    current_s curA;//失压时刻A相电流
    power_s activePA;//失压时刻A相有功功率
    power_s reactivePA;//失压时刻A相无功功率
    powerFactor_s factorA;//失压时刻A相功率因数
    dlt645ElecEnergy_s positiveIncB;//失压期间B相正向有功电能增量
    dlt645ElecEnergy_s reverseIncB;//失压期间B相反向有功电能增量
    dlt645ElecEnergy_s reactive1IncB;//失压期间B相组合无功1电能增量
    dlt645ElecEnergy_s reactive2IncB;//失压期间B相组合无功2电能增量
    voltage_s volB;//失压时刻B相电压
    current_s curB;//失压时刻B相电流
    power_s activePB;//失压时刻B相有功功率
    power_s reactivePB;//失压时刻B相无功功率
    powerFactor_s factorB;//失压时刻B相功率因数
    dlt645ElecEnergy_s positiveIncC;//失压期间C相正向有功电能增量
    dlt645ElecEnergy_s reverseIncC;//失压期间C相反向有功电能增量
    dlt645ElecEnergy_s reactive1IncC;//失压期间C相组合无功1电能增量
    dlt645ElecEnergy_s reactive2IncC;//失压期间C相组合无功2电能增量
    voltage_s volC;//失压时刻C相电压
    current_s curC;//失压时刻C相电流
    power_s activePC;//失压时刻C相有功功率
    power_s reactivePC;//失压时刻C相无功功率
    powerFactor_s factorC;//失压时刻C相功率因数
    dlt645ElecEnergy_s totalAh;//失压期间总安时数
    dlt645ElecEnergy_s AhA;//失压期间A相安时数
    dlt645ElecEnergy_s AhB;//失压期间B相安时数
    dlt645ElecEnergy_s AhC;//失压期间C相安时数
} lostVRec_s;

typedef struct { //全失压时间记录
    dlt645Date_s startTime;//开始时间
    current_s current;//电流值
    dlt645Date_s endTime;//结束时间
} allLostVRec_s;

typedef struct {//电压逆相序事件记录
    dlt645Date_s startTime;//发生时刻
    dlt645Date_s endTime;//结束时刻
    dlt645ElecEnergy_s positiveInc;//电压逆相序期间正向有功总电能增量
    dlt645ElecEnergy_s reverseInc;//电压逆相序期间反向有功总电能增量
    dlt645ElecEnergy_s reactive1Inc;//电压逆相序期间组合无功 1 总电能增量
    dlt645ElecEnergy_s reactive2Inc;//电压逆相序期间组合无功 2 总电能增量
    dlt645ElecEnergy_s positiveIncA;//电压逆相序期间 A 相正向有功电能增量
    dlt645ElecEnergy_s reverseIncA;//电压逆相序期间 A 相反向有功电能增量
    dlt645ElecEnergy_s reactive1IncA;//电压逆相序期间 A 相组合无功 1 电能增量
    dlt645ElecEnergy_s reactive2IncA;//电压逆相序期间 A 相组合无功 2 电能增量
    dlt645ElecEnergy_s positiveIncB;//电压逆相序期间 B 相正向有功电能增量
    dlt645ElecEnergy_s reverseIncB;//电压逆相序期间 B 相反向有功电能增量
    dlt645ElecEnergy_s reactive1IncB;//电压逆相序期间 B 相组合无功 1 电能增量
    dlt645ElecEnergy_s reactive2IncB;//电压逆相序期间 B 相组合无功 2 电能增量
    dlt645ElecEnergy_s positiveIncC;//电压逆相序期间 C 相正向有功电能增量
    dlt645ElecEnergy_s reverseIncC;//电压逆相序期间 C 相反向有功电能增量
    dlt645ElecEnergy_s reactive1IncC;//电压逆相序期间 C 相组合无功 1 电能增量
    dlt645ElecEnergy_s reactive2IncC;//电压逆相序期间 C 相组合无功 2 电能增量
} volReverseRec_s;

typedef struct {//电压不平衡事件记录
    dlt645Date_s startTime;//发生时刻
    dlt645Date_s endTime;//结束时刻
    u8 rate[2];//最大不平衡率
    dlt645ElecEnergy_s positiveInc;//电压不平衡期间正向有功总电能增量
    dlt645ElecEnergy_s reverseInc;//电压不平衡期间反向有功总电能增量
    dlt645ElecEnergy_s reactive1Inc;//电压不平衡期间组合无功 1 总电能增量
    dlt645ElecEnergy_s reactive2Inc;//电压不平衡期间组合无功 2 总电能增量
    dlt645ElecEnergy_s positiveIncA;//电压不平衡期间 A 相正向有功电能增量
    dlt645ElecEnergy_s reverseIncA;//电压不平衡期间 A 相反向有功电能增量
    dlt645ElecEnergy_s reactive1IncA;//电压不平衡期间 A 相组合无功 1 电能增量
    dlt645ElecEnergy_s reactive2IncA;//电压不平衡期间 A 相组合无功 2 电能增量
    dlt645ElecEnergy_s positiveIncB;//电压不平衡期间 B 相正向有功电能增量
    dlt645ElecEnergy_s reverseIncB;//电压不平衡期间 B 相反向有功电能增量
    dlt645ElecEnergy_s reactive1IncB;//电压不平衡期间 B 相组合无功 1 电能增量
    dlt645ElecEnergy_s reactive2IncB;//电压不平衡期间 B 相组合无功 2 电能增量
    dlt645ElecEnergy_s positiveIncC;//电压不平衡期间 C 相正向有功电能增量
    dlt645ElecEnergy_s reverseIncC;//电压不平衡期间 C 相反向有功电能增量
    dlt645ElecEnergy_s reactive1IncC;//电压不平衡期间 C 相组合无功 1 电能增量
    dlt645ElecEnergy_s reactive2IncC;//电压不平衡期间 C 相组合无功 2 电能增量
} volNotBalanceRec_s;

typedef lostVRec_s
phaseBrokenRec_s;//TEC扩展协议要求采集电能表三相断相统计数据及最近一次断相记录

//到 DI = 03 09 00 01, 事件记录太多了, 有时间慢慢补充


/***********事件类数据类型end***********/



/***********参变量类数据类型start***********/
/*
 * 有的只读, 有的读写
 */

typedef struct {//年月日星期, 读写
    comBCD_s    WW;
    comBCD_s    DD;
    comBCD_s    MM;
    comBCD_s    YY;
} YYMMDDWW_s;

typedef struct {//时分秒, 读写
    comBCD_s    ss;
    comBCD_s    mm;
    comBCD_s    hh;
} hhmmss_s;



/***********参变量类数据类型end***********/

/******************************645-07协议end************************************/


/******************************状态字、特征字、模式字、错误信息字start************************************/

typedef struct { //电表运行状态字 1
    u16        swt     :1;//外置开关控制方式, 0-电平, 1-脉冲
    u16        dem     :1;//需量计算方式, 0-滑差, 1-区间
    u16        batTM   :1;//时钟电池, 0-正常, 1-欠压
    u16        batRD   :1;//停电抄表电池, 0-正常, 1-欠压
    u16        actDir  :1;//有功功率方向, 0-正向, 1-反向
    u16        reaDir  :1;//无功功率方向, 0-正向, 1-反向
    u16        resv    :10;//保留
} runCode1_s;

typedef struct { //电表运行状态字 2
    u16        actDirA  :1;//A相有功功率方向, 0-正向， 1-反向, 下同
    u16        actDirB  :1;
    u16        actDirC  :1;
    u16        resv0     :1;//保留
    u16        reactDirA  :1;
    u16        reactDirB  :1;
    u16        reactDirC  :1;
    u16        resv1      :9;//保留
} runCode2_s;

typedef struct { //电表运行状态字 3
    u16        curInterval  :1;//当前运行时段, 0-第一套， 1-第二套
    u16        pwrMethod    :2;//供电方式, 00-主电源， 01-辅助电源， 10-电池供电
    u16        allowProg    :1;//编程允许, 0-禁止， 1-许可
    u16        relaySt      :1;//继电器状态, 0-通， 1-断
    u16        runTmArea    :1;//当前运行时区, 0-第一套， 1-第二套
    u16        relayCmd     :1;//继电器命令状态, 0-通，1-断
    u16        preTripAlarm :1;//预跳闸报警状态, 0-无，1-有
    u16        meterType    :2;//电能表类型, 00 非预付费表,01 电量型预付费表,10 电费型预付费表
    u16        price         :1;//当前运行费率电价, 0 第一套， 1 第二套
    u16        curLadder     :1;//当前阶梯,  0 第一套， 1 第二套
    u16        resv1         :4;//保留
} runCode3_s;

typedef struct { //电表运行状态字 4~6, ABC相故障状态字, 0代表无此类故障， 1代表当前发生此类故障
    u16        lostV:1;//失压
    u16        lackV:1;//欠压
    u16        overV:1;//过压
    u16        lostI:1;//失流
    u16        overI:1;//过流
    u16        overL:1;//过载
    u16        revFlow:1;//潮流反向
    u16        brkPhase:1;//断相
    u16        brkI:1;//断流
    u16        rsv:7;//保留
} runCode4_s;

typedef runCode4_s runCode5_s;//电表运行状态字 5
typedef runCode4_s runCode6_s;//电表运行状态字 6

typedef struct { //电表运行状态字 7
    u16        volResv:1;//电压逆相序
    u16        curResv:1;//电流逆相序
    u16        volunbal:1;//电压不平衡
    u16        curunbal:1;//电流不平衡
    u16        assistBatLost:1;//辅助电源失电
    u16        lostPwr:1;//掉电
    u16        overDemand:1;//需量超限
    u16        factorDown:1;//总功率因数超下限
    u16        curVeryunbal:1;//电流严重不平衡
    u16        resv:7;//保留
} runCode7_s;

typedef struct { //有功组合方式特征字
    u8        posActAdd:1;//正向有功, 0 不加， 1 加
    u8        posActMinus:1;//正向有功, 0 不减， 1 减
    u8        negActAdd:1;//反向有功, 0 不加， 1 加
    u8        negActMinus:1;//反向有功, 0 不减， 1 减
    u8        rsv:4;//保留
} actAssemWay_s;

typedef struct { //无功组合方式 1、 2 特征字
    u8        IAdd:1;//I 象限, 0 不加， 1 加
    u8        IMinus:1;//I 象限, 0 不减， 1 减
    u8        IIAdd:1;//II 象限, 0 不加， 1 加
    u8        IIMinus:1;//II 象限, 0 不减， 1 减
    u8        IIIAdd:1;//III 象限, 0 不加， 1 加
    u8        IIIMinus:1;//III 象限, 0 不减， 1 减
    u8        IVAdd:1;//IV 象限, 0 不加， 1 加
    u8        IVMinus:1;//IV 象限, 0 不减， 1 减
} reactAssemWay_s;

typedef struct { //周休日特征字, 0代表休息， 1代表工作
    u8 weekday :1;
    u8 Monday :1;
    u8 Tuesday :1;
    u8 Wednesday :1;
    u8 Thursday :1;
    u8 Friday :1;
    u8 Saturday :1;
    u8 rsv :1;
} weekendCode_s;

typedef struct { //通信速率特征字, 注： 0代表非当前接口通信速率， 1代表当前接口通信速率，特征字仅在某一位为1时有效。
    u8 rsv0 :1;
    u8 b600 :1;
    u8 b1200 :1;
    u8 b2400 :1;
    u8 b4800 :1;
    u8 b9600 :1;
    u8 b19200 :1;
    u8 rsv1 :1;
} baudrateCode_s;

typedef struct { //负荷记录模式字, 0代表不记录此类数据， 1代表记录此类数据。
    u8 volCurFreq :1;
    u8 actReactPwr:1;
    u8 factor:1;
    u8 actReactEnergy:1;
    u8 I2IVReactEnergy:1;
    u8 curDemand:1;
    u8 rsv:2;
} curveCode_s;

typedef struct { //冻结数据模式字, 0代表不记录此类数据， 1代表记录此类数据。
    u8 posActE:1;//正向有功电能
    u8 negActE:1;//反向有功电能
    u8 assemReactE1:1;//组合无功 1电能
    u8 assemReactE2:1;//组合无功 2电能
    u8 I2IVReactE:1;//四象限无功电能
    u8 posActDemand:1;//正向有功最大需量及发生时间
    u8 negActDemand:1;//反向有功最大需量及发生时间
    u8 var:1;//变量
} freezeCode_s;

typedef freezeCode_s
timeingFreezeCode_s;//定时冻结数据模式字, 0代表不记录此类数据， 1代表记录此类数据。
typedef freezeCode_s
instantFreezeCode_s;//瞬时冻结数据模式字, 0代表不记录此类数据， 1代表记录此类数据。
typedef freezeCode_s
promiseFreezeCode_s;//约定冻结数据模式字, 0代表不记录此类数据， 1代表记录此类数据。
typedef freezeCode_s
dayFreezeCode_s;//日冻结数据模式字, 0代表不记录此类数据， 1代表记录此类数据。

typedef struct { //整点冻结数据模式字
    u8 posActE:1;//正向有功电能
    u8 negActE:1;//反向有功电能
    u8 rsv:6;
} hourFreezeCode_s;


typedef struct { //安全认证错误信息字 SERR, 0代表无相应错误发生， 1代表相应错误发生。
    u16    otherErr:1;//其他错误
    u16    repeatRecharge:1;//重复充值
    u16    esamErr:1;//esam验证失败
    u16    authErr:1;//身份验证失败
    u16    clientNoMismatch:1;//客户编号不匹配
    u16    rechargeTimesErr:1;//充值次数错误
    u16    overHord:1;//购电超囤积
    u16    rsv:9;//保留
} secureCode_s;

typedef struct { //错误信息字ERR
    u8    otherErr:1;//其他错误
    u8    noData:1;//无请求数据
    u8    pwdwrOrNotAuth:1;//密码错误或未授权
    u8    cannotChgBaud:1;//通信速率不能更改
    u8    yearAreaOver:1;//年时区数超
    u8    dayAreaOver:1;//日时段数超
    u8    rateOver:1;//费率数超
    u8    rsv:1;//保留
} errCode_s;

/******************************状态字、特征字、模式字、错误信息字end************************************/

/************************************扩展645数据类型start***************************************/

/*
 * 读写系统时间
 */
typedef struct {
    u8 ss; //秒
    u8 mm; //分
    u8 hh; //时
    u8 WW; //星期
    u8 DD; //日
    u8 MM; //月
    u8 YY; //年
} dlt645SysTime_s;

/*
 * 采集器M-Bus接口供电休眠时间,
 * BCD格式1byte, 单位min.
 * 00: 持续供电,
 * 其他值: 无通信后相应时间进入休眠模式.
 */


/*
 * 采集器无线接口数据老化时间,
 * BCD格式2byte, 单位min
 * 0: 不保存
 * 其他值: 按设置值保持
 * 当采集器收到无线接口的表数据
 * 后, 按设置值保存数据
 */
typedef struct {
    comBCD_s l;
    comBCD_s h;
} dlt645WireSave_s;

/*
 * 表计无通信时的档案保存时间,
 * BCD格式1byte, 单位d.
 * 0: 删除现有档案, 重新建档,
 *    档案保存时间不变.
 * 其他值: 按照设置的时间保存,
 *        在保存期限内, 若没有
 *        通信记录, 则删除该
 *        表计的档案.
 *        默认值30天.
 */



/*
 * 采集器地址, BCD格式, 6byte.
 * 只支持点对点设置.
 */
typedef struct {
    comBCD_s  address[6];
} acqLogicAddress_s;

/*
 * ##全透明转发报文
 * 1. 控制码 = 0x11
 * 2. 数据标识 = 0xE5E50000
 * 3. 数据域长度 = 14 + 转发内容长度
 * 4. 从站正常应答, 数据域为DI+表计应答报文
 *    表计若无应答或应答错误, 则采集器不对主站进行应答
 * 5. 报文除数据标识部分需进行加减0x33处理外, 其余
 *    部分不需进行加减0x33处理
 * 注1: 为防止干扰, 采集器应该对应答的数据进行验证, 报文长度不短于12Byte.
 * 注2: 返回的报文不进行加减0x33处理, 直接封包.
 */

typedef union {                  //通信端口参数配置字
    u8 u8b;
    struct {
#define DLT645_DATABITS_5    0 //5位数据位
#define DLT645_DATABITS_6    1 //6位数据位
#define DLT645_DATABITS_7    2 //7位数据位
#define DLT645_DATABITS_8    3 //8位数据位
        u8  databits    :2;
#define DLT645_PARITY_E    0 //偶校验
#define DLT645_PARITY_O    1 //奇校验
        u8  parity      :1;
#define DLT645_PARITY_HASNOT    0 //无校验位
#define DLT645_PARITY_HAS       1 //有校验位
        u8  chkOrNot    :1;
#define DLT645_STOPBIT_1    0 //1位停止位
#define DLT645_STOPBIT_2    1 //2位停止位
        u8  stopbits    :1;
#define DLT645_BAUDRATE_ADJ     0 //波特率-自适应
#define DLT645_BAUDRATE_1200    1 //波特率-1200
#define DLT645_BAUDRATE_2400    2 //波特率-2400
#define DLT645_BAUDRATE_4800    3 //波特率-4800
#define DLT645_BAUDRATE_9600    4 //波特率-9600
#define DLT645_BAUDRATE_19200   5 //波特率-19200
#define DLT645_BAUDRATE_38400   6 //波特率-38400
#define DLT645_BAUDRATE_57600   7 //波特率-57600
        u8  baudrate    :3;
    } cfg;
} dlt645ComCfg_s;

typedef enum {
    e_645rs485Port = 1,
    e_645MbusPort,
    e_645wirelessPort
} dlt645Port_e;

typedef struct {                         //透明转发全部报文0xE5E50000
    di645_u          di;
#define DLT645_PORT_RS485BASE    (0U)  //RS485端口的起始索引, 0代表尝试切换1~9号端口
#define DLT645_PORT_MBUSBASE     (10U) //MBUS端口的起始索引, 10代表尝试切换1~9号端口
#define DLT645_PORT_MICROBASE    (20U) //微功率无线端口的起始索引, 20代表尝试切换1~10号端口, 协议规定最大端口为30
    u8               port;
    dlt645ComCfg_s fmt;
    u8               addr[8];            //用于端口与表计关系绑定处理
    u8               b[256];             //全透明转发报文
    u8               len;                //全透明转发报文长度
} dlt645TransAll_s;
typedef dlt645TransAll_s* dlt645TransAll_p;

#define DLT645_TRANSALL_MIN    ((((dlt645TransAll_p)0)->b)-&(((dlt645TransAll_p)0)->di.list[0]))

/*
 * ##转发CJ/T 188转换报文
 * 1. 控制码 = 0x11
 * 2. 数据标识 = 0xE5E50001
 * 3. 数据域长度 = 14 + 转发内容长度
 * 4. 从站正常应sudo apt-get install fcitx-config-gtk答, 数据域为DI+表计应答报文
 *    表计若无应答或应答错误, 则采集器不sudo apt-get install fcitx-config-gtk对主站进行应答
 * 5. 报文除数据标识部分需进行加减0x33处理外, 其余
 *    部分不需进行加减0x33处理
 * 注: 采集器将表计响应的报文转换为标准CJ/T188报文后,
 *     封包后发给集中器.
 */
typedef dlt645TransAll_s dlt645Trans188_s;
typedef dlt645Trans188_s* dlt645Trans188_p; //188转换报文的格式与全透明转发的格式一致.

#define DLT645_TRANS188_MIN    DLT645_TRANSALL_MIN


/*
 * ##文件传输报文
 * 1. 控制码 = 0x?? , 协议未定义, 暂用CTL_645_WDATA       0x14
 * 2. 数据标识 = 0xE5E50300
 * 3. 从站应答: 正确接收后, 返回4字节的文件段标识索引号,
 *              若报文校验错误, 则返回0xFFFFFFFF
 */

typedef struct {
#define DLT645_FILEDI_CLEAR    0x00 //清除传输文件, 恢复到升级前状态
#define DLT645_FILEDI_UPDATE   0x01 //采集器升级文件
#define DLT645_FILEDI_UPDATE1  0x10 //采集器升级文件
    u8    fileDi; //文件标识
#define DLT645_FILEPRO_SM      0x00 //起始或中间帧
#define DLT645_FILEPRO_END     0x01 //结束帧
    u8    filePro; //文件属性
#define DLT645_FILECMD_FRM     0x00 //报文方式传输
    u8    fileCmd; //文件指令
    u16   cnt;     //文件总段数
    u32   idx;     //文件段索引
    u16   len;     //当前文件段长度
    u8    d[256];  //文件段
} dlt645FileTrans_s;

/*
 * ##文件传输未收到数据段响应报文
 * 1. 控制码 = 0x?? , 协议未定义, 暂用CTL_645_FILETRANS       0x1E
 * 2. 数据标识 = 0xE5E50301
 * 3. 每1024个文件段, 组成一个文件组, 从0开始编号, 采集器返回第一个
 *    没有收全的组. 128个字节共1024位, D0~D1023位对应当前组的第
 *    0~1023个数据段, 0-ctl已收到, 1-未收到.
 *    但是有一个疑问, u16类型最大可表示的整数是2^16-1, 文件总段数不过
 *    65535, 每1024个段组成一个文件组, 那么最多可组 2^16/2^10 = 2^6,
 *    即64个文件组, 但协议偏要用两个字节来表示组序号, 显得浪费.
 */

typedef struct { //一个文件组中每个段的接收标识
    u16 grpId;//组号
    u8 b[128];//数据段标志
} dlt645FileFlg_s;

#define DLT645_FILE_CLRBIT(b, idx)        CLEAR_BIT((d)[(idx)/8], (idx)%8)            //设置数组b的第idx位为0
#define DLT645_FILE_SETBIT(b, idx)        SET_BIT((d)[(idx)/8], (idx)%8)              //设置数组b的第idx位为1
#define DLT645_FILE_GROUP_IDX(dump)       ((dump)/1024 + (((dump)%1024) ? 1 : 0))     //获取段dump的组序号

/************************************扩展645数据类型end***************************************/


/************************************645数据类型start***************************************/
#define DLT645_DATATYPE_UNKNOWNORERR  0  //未知或错误, 或未赋值, 用于检测数据类型是否定义

typedef struct {   //读数据, C=0x11
    enum {
        e_645downRDInvalidOrNotSet = 0,//不合法或初始化后未使用
        e_645downRDDI = 1,          //下行报文, 读取数据项
        e_645downRDCurve,       //下行报文, 读取负荷记录
        e_645downRDCurvePoint,   //下行报文, 读取特定时间点的负荷记录
        e_645exdownRDAllTrans,      //下行报文, 全透明转发
        e_645exdownRD188Trans,     //下行报文, 转发CJ/T188报文
        e_645exdownRDSystime,//下行报文, 扩展645协议, 读取系统日期时间
        e_645exdownRDMBusPwr,//下行报文, Mbus供电时间
        e_645exdownRDWireLessDataTime,//下行报文, 无线接口数据老化时间
        e_645exdownRDMeterInfoSaveTime,//下行报文, 表计无通信, 档案保存时间
        e_645exdownRDAcqAddr,//下行报文, 采集器地址
        e_645exdownRDDumpNotRcv,//下行报文, 文件传输, 未收到数据段
        e_645exupRDSystime,//上行报文, 扩展645协议, 读取系统日期时间
        e_645exupRDMBusPwr,//上行报文, Mbus供电时间
        e_645exupRDWireLessDataTime,//上行报文, 无线接口数据老化时间
        e_645exupRDMeterInfoSaveTime,//上行报文, 表计无通信, 档案保存时间
        e_645exupRDAcqAddr,//上行报文, 采集器地址
        e_645exupRDAllTrans,      //上行报文, 全透明转发
        e_645exupRD188Trans,     //上行报文, 转发CJ/T188报文
        e_645exupRDDumpNotRcv//上行报文, 文件传输, 未收到数据段
    } e;
    union {
        dlt645downRDDI_s rdDI;
        dlt645downRDCurve_s rdCurve;
        dlt645downRDCurvePoint_s rdCurvePoint;
        dlt645TransAll_s allTransDown;
        dlt645Trans188_s cj188TransDown;
        dlt645SysTime_s systime;
        comBCD_s  mbusPwrTime;
        dlt645WireSave_s wireAgeTime;
        comBCD_s  meterInfoSaveTime;
        comBCD_s  address[6];
        dlt645FileFlg_s upRDNotRcv;
    } u;
} dlt645ReadData_s;

typedef struct {   //写数据, C=0x14
    enum {
        e_645exdownWRInvalidOrNotSet = 0,//不合法或初始化后未使用
        e_645exdownWRSystime,//下行报文, 扩展645协议, 写入系统日期时间
        e_645exdownWRMBusPwr,//下行报文, 写入Mbus供电时间
        e_645exdownWRWireLessDataTime,//下行报文, 写入无线接口数据老化时间
        e_645exdownWRMeterInfoSaveTime,//下行报文, 写入表计无通信, 档案保存时间
        e_645exdownWRAcqAddr,//下行报文, 写入采集器地址
        e_645exdownWRInitPara,//下行报文, 参数初始化
        e_645exdownWRInitData//下行报文, 数据初始化
    } e;
    union {
        dlt645SysTime_s systime;
        comBCD_s  mbusPwrTime;
        dlt645WireSave_s wireAgeTime;
        comBCD_s  meterInfoSaveTime;
        acqLogicAddress_s  address;
    } u;
} dlt645WriteData_s;

/*
 * dlt645数据类型
 * 外部根据e来判断解析到的具体数据类型
 * 再进一步根据具体的数据类型来使用.
 * 解析数据时, 应用层的数据类型是根据
 * 控制字中的功能码和上/下行报文来进行
 * 组织的, 所以程序要按照功能码和上/下
 * 行报文来划分解析具体的数据结构.
 * 这个结构(包括具体的读取数据结构,
 * 写数据的结构)中直接使用具体的数据
 * 缓冲区来存取数据, 这样做可以省略另
 * 行开辟内存和释放内存的麻烦, 也不必
 * 担心内存申请后的泄漏问题, 但是比较
 * 浪费空间.
 * 另一种做法是,
 * dlt645Data_s内部的u的成员, 只是相
 * 应类型的指针, 解析到具体使用哪种类型
 * 时, 去申请相应类型的内存空间, 外部调
 * 用函数负责释放内存空间.
 */
typedef struct {
    enum {
        e_645InvalidOrNotSet = 0,  //不合法或初始化后未使用
        e_645broadcastTime = 1,    //广播校时
        e_645readData,
        e_645readSuccedData,
        e_645writeData,
        e_645readAddress,
        e_645writeAddress,
        e_645freezeData,
        e_645changeBaudrate,
        e_645modifyPasswd,
        e_645clearDemand,
        e_645clearData,
        e_645clearEvent,
        e_645exFileTrans,
    } e;
    union {
        dlt645ReadData_s readData;
        dlt645WriteData_s writeData;
        dlt645FileTrans_s fileTrans;
    } u;
    s16 flag;							//是否需要返回报文
} dlt645Data_s;
typedef dlt645Data_s* dlt645Data_p;
/************************************645数据类型end***************************************/

#pragma pack(pop)

#ifdef __cplusplus
extern "C" {
#endif

#include "libutil/basedef.h"

#ifdef __cplusplus
}
#endif

#endif // LIBDLT645_H
