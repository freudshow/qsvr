#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libdlt645.h"


u8 dlt645Sum(u8* buf, u16 bufSize)
{
    u16 i = 0;
    u8 sum = 0;

    for (i = 0, sum = 0; i < bufSize; i++)
        sum += buf[i];

    return sum;
}

void dlt645Add33(u8* buf, u16 bufSize)
{
    u16 i = 0;

    for (i = 0; i < bufSize; i++)
        buf[i] += DLT645_BIAS;
}

void dlt645Minus33(u8* buf, u16 bufSize)
{
    u16 i = 0;

    for (i = 0; i < bufSize; i++)
        buf[i] -= DLT645_BIAS;
}

/***************************************
 * CJ/T188 报文检查, 检查以下内容:
 * 1. 检查bufSize
 * 2. 检查开始符0x68和第2个0x68
 * 3. 检查地址域是否是6个, 且是否是压缩BCD码
 * 4. 检查报文长度是否与报文内声称的长度相等
 * 5. 检查结束符
 * 6. 检查校验值
 ***************************************/
s8 dlt645Chk(u8* buf, u16 bufSize)
{
    u8 i = 0;

    if ( NULL == buf || 0 == bufSize)
        return DLT645_ERR_RCVD_LOST;

    if (bufSize < DLT645_MINSIZE)
        return DLT645_ERR_RCVD_LOST;
    if (buf[0] != DLT645_START_CHAR || buf[7] != DLT645_START_CHAR)
        return DLT645_ERR_LOST1_0x68;
    for (i = 0; i < 6; i++) {
        if (!isCOMBCD(buf[i + 1]))
            return DLT645_ERR_RCVD_LOST;
    }
    if (bufSize != (buf[9] + DLT645_MINSIZE))
        return DLT645_ERR_RCVD_LOST;
    if (buf[bufSize - 1] != DLT645_END_CHAR)
        return DLT645_ERR_LOST_0x16;
    if (dlt645Sum(&buf[0], bufSize - 2) != buf[bufSize - 2])
        return DLT645_ERR_SUM_ERROR;   // 校验失败
    return DLT645_CHK_SUC;
}


static void printAddress(u8* addr)
{
    fprintf(stderr,
            "logic address: %02X%02X%02X%02X%02X%02X\n",
            addr[5], addr[4], addr[3],
            addr[2], addr[1], addr[0]);
}

static void printCtl(ctl645_u ctl)
{
    fprintf(stderr, "direction: %d, valid: %d, \n", ctl.ctl.dir, ctl.ctl.valid);
    switch (ctl.ctl.func) {
        case CTL_645_REV:
            fprintf(stderr, "ctr: %d, reserve\n", ctl.ctl.func);
            break;
        case CTL_645_BROADCAST:
            fprintf(stderr, "ctr: %d, broad cast\n", ctl.ctl.func);
            break;
        case CTL_645_RDATA:
            fprintf(stderr, "ctr: %d, read data\n", ctl.ctl.func);
            break;
        case CTL_645_RSUCD:
            fprintf(stderr, "ctr: %d, read succeed data\n", ctl.ctl.func);
            break;
        case CTL_645_RADDR:
            fprintf(stderr, "ctr: %d, read address\n", ctl.ctl.func);
            break;
        case CTL_645_WDATA:
            fprintf(stderr, "ctr: %d, write data\n", ctl.ctl.func);
            break;
        case CTL_645_WADDR:
            fprintf(stderr, "ctr: %d, write address\n",
                    ctl.ctl.func);
            break;
        case CTL_645_FRZ:
            fprintf(stderr, "ctr: %d, freeze command\n",
                    ctl.ctl.func);
            break;
        case CTL_645_WBAUD:
            fprintf(stderr, "ctr: %d, write baudrate\n",
                    ctl.ctl.func);
            break;
        case CTL_645_WPASSWD:
            fprintf(stderr, "ctr: %d, write passwd\n",
                    ctl.ctl.func);
            break;
        case CTL_645_CLRDEM:
            fprintf(stderr, "ctr: %d, clear max demand\n",
                    ctl.ctl.func);
            break;
        case CTL_645_CLRMETER:
            fprintf(stderr, "ctr: %d, clear meter data\n",
                    ctl.ctl.func);
            break;
        case CTL_645_CLRENT:
            fprintf(stderr, "ctr: %d, clear event\n",
                    ctl.ctl.func);
            break;
        case CTL_645_FILETRANS:
            fprintf(stderr, "ctr: %d, clear event\n",
                    ctl.ctl.func);
            break;
        default:
            fprintf(stderr, "ctr: %d, unknown function code or vendor defined\n",
                    ctl.ctl.func);
            break;
    }
}


/*
 * 解包645报文
 * 先解出链路层数据,
 * 在根据控制码和数据层中的
 * 数据标识符来解应用层数据.
 */
s8 unpackdlt645(u8* buf, u16 bufSize, DLT645_p pdlt645)
{
    s16 i = 0;
    s8 ret = DLT645_CHK_SUC;
    u8* p = buf;

    if (NULL == pdlt645 || NULL == buf || 0 == bufSize)
        return DLT645_ERR_RCVD_LOST;

    while(*p != DLT645_START_CHAR && i < bufSize) {
        p++;    //找报文头
        i++;
    }
    bufSize -= i;

    ret = dlt645Chk(p, bufSize);
    if (ret == DLT645_CHK_SUC) {
        memcpy(&(pdlt645->head), p, sizeof(head645_s));
        p += sizeof(head645_s);

        printAddress(pdlt645->head.addr);
        printCtl(pdlt645->head.ctl);

        memcpy(&pdlt645->d[0], p, pdlt645->head.len);
        p += pdlt645->head.len;
    }

    return ret;
}

s8 DLT645_parseDownReadData(DLT645_p pdlt645, dlt645Data_p pData)
{
    s8 ret = DLT645_CHK_SUC;
    di645_u di;

    di.u32b = 0;
    if (pdlt645->head.len < sizeof(di645_u))
        return DLT645_ERR_RCVD_LOST;

    memcpy(&di, pdlt645->d, sizeof(di645_u));
    dlt645Minus33(di.list, 4);//协议没有指出数据标识不需要加减0x33
    switch(di.u32b) {
        case DLT645_DI_CUR_TIME://下行读取系统日期时间, 只有数据标识
            pData->u.readData.e = e_645exdownRDSystime;
            break;
        case DLT645_DI_TRANSMIT:
            pData->u.readData.e = e_645exdownRDAllTrans;
            pData->u.readData.u.allTransDown.len = (pdlt645->head.len-DLT645_TRANSALL_MIN);
            memcpy(&pData->u.readData.u.allTransDown.di, pdlt645->d,DLT645_TRANSALL_MIN);
            memcpy(&(pData->u.readData.u.allTransDown.b), (pdlt645->d+DLT645_TRANSALL_MIN),
                   pData->u.readData.u.allTransDown.len);
            break;
        case DLT645_DI_TRANS188:
            pData->u.readData.e = e_645exdownRD188Trans;
            pData->u.readData.u.cj188TransDown.len = (pdlt645->head.len-DLT645_TRANSALL_MIN);
            memcpy(&pData->u.readData.u.allTransDown.di, pdlt645->d,DLT645_TRANSALL_MIN);
            memcpy(&(pData->u.readData.u.cj188TransDown.b), (pdlt645->d+DLT645_TRANSALL_MIN),
                   pData->u.readData.u.cj188TransDown.len);
            //TODO: 直接从标准CJ188报文转为对应厂商报文
            break;
        case DLT645_DI_MBUS_SLEEP_TIME://下行读取Mbus供电休眠时间, 只有数据标识
            pData->u.readData.e = e_645exdownRDMBusPwr;
            break;
        case DLT645_DI_WIRELESS_DATA_AGING://下行读取无线接口数据老化时间, 只有数据标识
            pData->u.readData.e = e_645exdownRDWireLessDataTime;
            break;
        case DLT645_DI_REC_SAVE_TIME://下行读取无通信后的档案保存时间, 只有数据标识
            pData->u.readData.e = e_645exdownRDMeterInfoSaveTime;
            break;
        case DLT645_DI_LOGIC_ADDRESS://下行读取采集器逻辑, 只有数据标识
            pData->u.readData.e = e_645exdownRDAcqAddr;
            break;
        case DLT645_DI_FILE_NO_DATA://下行读取文件传输未接收到数据段, 只有数据标识
            pData->u.readData.e = e_645exdownRDDumpNotRcv;
            break;
        default:
            pData->u.readData.e = DLT645_DATATYPE_UNKNOWNORERR;
            ret = DLT645_ERR_DATA_FMT;
            break;
    }

    return ret;
}

/*
 * 获取仪表数据宏定义
 * @pdlt645: 指向DLT645_s结构体的指针
 * @e: 电表数据类型的枚举变量
 * @etype: 电表数据类型的枚举类型
 * @type: 电表数据的具体数据类型名
 * @member: 电表数据的具体数据类型的成员名
 * @ret: 返回值
 */
#define GET_645METER_DATA_WITH_DI(pdlt645, e, etype, type, member, ret)    do{\
                                                                        if(pdlt645->head.len != sizeof(type)) {\
                                                                            ret = DLT645_ERR_DATA_FMT;\
                                                                        } else{\
                                                                            e = etype;\
                                                                            memcpy((u8*)&(member), \
                                                                                    (&(pdlt645->d[0])+sizeof(di645_u)), \
                                                                                    sizeof(type));\
                                                                        }\
                                                                    }while(0)


s8 DLT645_parseDownWriteData(DLT645_p pdlt645, dlt645Data_p pData)
{
    s8 ret = DLT645_CHK_SUC;
    di645_u di;

    di.u32b = 0;
    if (pdlt645->head.len < sizeof(di645_u))
        return DLT645_ERR_RCVD_LOST;

    memcpy(&di, pdlt645->d, sizeof(di645_u));

    switch (di.u32b) {
    case DLT645_DI_CUR_TIME:    //下行写入系统日期时间
        GET_645METER_DATA_WITH_DI(pdlt645, pData->u.writeData.e,
                e_645exdownWRSystime, dlt645SysTime_s,
                pData->u.writeData.u.systime, ret);
        break;
    case DLT645_DI_MBUS_SLEEP_TIME:    //下行写入Mbus供电休眠时间
        GET_645METER_DATA_WITH_DI(pdlt645, pData->u.writeData.e,
                e_645exdownWRMBusPwr, comBCD_s,
                        pData->u.writeData.u.mbusPwrTime, ret);
        break;
    case DLT645_DI_WIRELESS_DATA_AGING:    //下行写入无线接口数据老化时间
        GET_645METER_DATA_WITH_DI(pdlt645, pData->u.writeData.e,
                e_645exdownWRWireLessDataTime, dlt645WireSave_s,
                                pData->u.writeData.u.wireAgeTime, ret);
        break;
    case DLT645_DI_REC_SAVE_TIME:    //下行写入表计无通信, 档案保存时间
        GET_645METER_DATA_WITH_DI(pdlt645, pData->u.writeData.e,
                e_645exdownWRMeterInfoSaveTime, comBCD_s,
                                        pData->u.writeData.u.meterInfoSaveTime, ret);
        break;
    case DLT645_DI_LOGIC_ADDRESS:    //下行写入采集器地址
        GET_645METER_DATA_WITH_DI(pdlt645, pData->u.writeData.e,
                        e_645exdownWRWireLessDataTime, acqLogicAddress_s,
                                        pData->u.writeData.u.address, ret);
        break;
    case DLT645_DI_INIT_PARA:
        pData->u.writeData.e = e_645exdownWRInitPara;
        break;
    case DLT645_DI_INIT_DATA:
        pData->u.writeData.e = e_645exdownWRInitData;
        break;
    default:
        ret = DLT645_ERR_DATA_FMT;
        break;
    }

    return ret;
}

s8 DLT645_parseDownData(DLT645_p pdlt645, dlt645Data_p pData)
{
    s8 ret = DLT645_CHK_SUC;

    switch (pdlt645->head.ctl.ctl.func) {
        case CTL_645_REV://保留
            break;
        case CTL_645_BROADCAST://广播校时
            pData->e = e_645broadcastTime;
            break;
        case CTL_645_RDATA://读数据
            pData->e = e_645readData;
            ret = DLT645_parseDownReadData(pdlt645, pData);
            break;
        case CTL_645_RSUCD://读后续数据
            pData->e = e_645readSuccedData;
            break;
        case CTL_645_RADDR://读通信地址
            pData->e = e_645readAddress;
            break;
        case CTL_645_WDATA://写数据
            pData->e = e_645writeData;
            ret = DLT645_parseDownWriteData(pdlt645, pData);
            break;
        case CTL_645_WADDR://写通信地址
            pData->e = e_645writeAddress;
            break;
        case CTL_645_FRZ://冻结命令
            pData->e = e_645freezeData;
            break;
        case CTL_645_WBAUD://更改通信速率
            pData->e = e_645changeBaudrate;
            break;
        case CTL_645_WPASSWD://修改密码
            pData->e = e_645modifyPasswd;
            break;
        case CTL_645_CLRDEM://最大需量清零
            pData->e = e_645clearDemand;
            break;
        case CTL_645_CLRMETER://电表清零
            pData->e = e_645clearData;
            break;
        case CTL_645_CLRENT://事件清零
            pData->e = e_645clearEvent;
            break;
        case CTL_645_FILETRANS://文件传输
            pData->e = e_645exFileTrans;
            break;
        default:
            ret = DLT645_ERR_CTL_FUNC_NDEF;
            break;
    }

    return ret;
}

/*
 * CJ/T188 应用层数据内容解析
 * 解析数据时, 先查看功能码,
 * 再查看上/下行方向
 * 如果是上行报文, 则再查看
 * 是正常应答还是异常应答.
 * TODO: 要加上需不需要返回报文的flag, 并把报文组好, 外部调用时, 只需判断flag
 * flag = 0, 不需要执行后续动作
 * flag = 1, 需要转发报文, 转发的报文在本协议中要组好, 不需要外部函数关注是否需要协议转换
 * flag = 2, 需要读取系统参数, 并给予应答
 * flag = 3, 需要写入数据, 并给予应答
 * flag = 4, 需要删除系统参数, 并给予应答
 * flag = 5, 需要清空数据, 并给予应答
 * flag = 6, 需要文件传输, 并给予应答
 */
s8 DLT645_parseData(DLT645_p pdlt645, dlt645Data_p pData)
{
    s8 ret = DLT645_CHK_SUC;

    switch(pdlt645->head.ctl.ctl.dir) {
        case CTL_645_DOWN:
            ret = DLT645_parseDownData(pdlt645, pData);
            break;
        case CTL_645_UP:
            break;
        default:
            break;
    }

    return ret;
}


s8 DLT645_Compose(DLT645_p pdlt645, u8* buf, u16* bufSize, u8 preCnt)
{
    if(NULL == pdlt645 || NULL == buf ||
       NULL == bufSize || *bufSize < DLT645_MINSIZE)
        return FALSE;

    u8* p = buf;
    //前导符目前遇到最长的有13个, 取整到20
    if(preCnt < 20) {
        memset(p, DLT645_PREFIX, preCnt);
        p += preCnt;
    } else {
        return FALSE;
    }

    //头部
    memcpy(p, &pdlt645->head, sizeof(head645_s));
    p += sizeof(head645_s);
    //数据域
    memcpy(p, &pdlt645->d, pdlt645->head.len);
    p += pdlt645->head.len;
    //校验和
    *p = dlt645Sum(buf+preCnt, (p-buf)-preCnt);
    p++;
    //结束符
    *p = DLT645_END_CHAR;
    p++;

    *bufSize = (p-buf);

    return TRUE;
}

/*
 * @pdlt645: 外部调用函数负责加入地址域
 * @buf: 上行报文缓冲区
 * @bufSize: 上行报文缓冲区长度
 * @mbuf: 表计返回的报文
 * @mbufSize: 表计返回的报文长度
 * @preCnt: 前导符个数
 */
s8 DLT645_upComposeTransAll(DLT645_p pdlt645, u8* buf, u16* bufSize, u8* mbuf, u16 mbufSize, u8 preCnt)
{
    if (NULL == pdlt645|| NULL == buf || NULL == bufSize ||
        NULL == mbuf || mbufSize < 12 || *bufSize < DLT645_MINSIZE)
        return FALSE;

    di645_u di;
    u8* p = pdlt645->d;

    di.u32b = 0;
    pdlt645->head.start1 = DLT645_START_CHAR;
    pdlt645->head.start2 = DLT645_START_CHAR;
    pdlt645->head.ctl.ctl.dir = CTL_645_UP;
    pdlt645->head.ctl.ctl.valid = CTL_645_VALID;
    pdlt645->head.ctl.ctl.succeed = CTL_645_NOSUCCEED;
    pdlt645->head.ctl.ctl.func = CTL_645_RDATA;

    di.u32b = DLT645_DI_TRANSMIT;
    dlt645Add33(&di.list[0], sizeof(di645_u));
    memcpy(p, &di, sizeof(di645_u));
    p += sizeof(di645_u);
    memcpy(p, mbuf, mbufSize);
    pdlt645->head.len = (sizeof(di645_u) + mbufSize);

    return DLT645_Compose(pdlt645, buf, bufSize, preCnt);
}


/*
 * @pdlt645: 外部调用函数负责加入地址域
 * @buf: 上行报文缓冲区
 * @bufSize: 上行报文缓冲区长度
 * @mbuf: 表计返回的报文转化成的标准188报文
 * @mbufSize: 表计返回的报文长度
 * @preCnt: 前导符个数
 */
s8 DLT645_upComposeTrans188(DLT645_p pdlt645, u8* buf, u16* bufSize, u8* mbuf, u16 mbufSize, u8 preCnt)
{
    if (NULL == pdlt645|| NULL == buf || NULL == bufSize ||
        NULL == mbuf || mbufSize < 12 || *bufSize < DLT645_MINSIZE)
        return FALSE;

    di645_u di;
    u8* p = pdlt645->d;

    di.u32b = 0;
    pdlt645->head.start1 = DLT645_START_CHAR;
    pdlt645->head.start2 = DLT645_START_CHAR;
    pdlt645->head.ctl.ctl.dir = CTL_645_UP;
    pdlt645->head.ctl.ctl.valid = CTL_645_VALID;
    pdlt645->head.ctl.ctl.succeed = CTL_645_NOSUCCEED;
    pdlt645->head.ctl.ctl.func = CTL_645_RDATA;

    di.u32b = DLT645_DI_TRANS188;
    dlt645Add33(&di.list[0], sizeof(di645_u));
    memcpy(p, &di, sizeof(di645_u));
    p += sizeof(di645_u);
    memcpy(p, mbuf, mbufSize);
    pdlt645->head.len = (sizeof(di645_u) + mbufSize);

    return DLT645_Compose(pdlt645, buf, bufSize, preCnt);
}

/*
 * @port: 端口类型, 见dlt645Port_e的定义
 * @no: 端口号
 * @return: 成功-返回对应的端口, 失败-返回0
 */
u8 getPort(dlt645Port_e port, u8 no)
{
    if (no  > 9)
        return 0;

    switch(port) {
        case e_645rs485Port:
            return (DLT645_PORT_RS485BASE+no);
        case e_645MbusPort:
            return (DLT645_PORT_RS485BASE+no);
        case e_645wirelessPort:
            return (DLT645_PORT_RS485BASE+no);
        default:
            return 0;
    }
}


/*
 * @pdlt645: 外部调用函数负责加入地址域
 * @buf: 下行报文缓冲区
 * @bufSize: 下行报文缓冲区长度
 * @preCnt: 前导符个数
 */
s8 DLT645_downComposeTransAll(DLT645_p pdlt645, dlt645TransAll_p pData, u8* buf, u16* bufSize, u8 preCnt)
{
    if (NULL == pdlt645|| NULL == buf || NULL == bufSize || *bufSize < DLT645_MINSIZE)
        return FALSE;

    pdlt645->head.start1 = DLT645_START_CHAR;
    pdlt645->head.start2 = DLT645_START_CHAR;
    pdlt645->head.ctl.ctl.dir = CTL_645_DOWN;
    pdlt645->head.ctl.ctl.valid = CTL_645_VALID;
    pdlt645->head.ctl.ctl.succeed = CTL_645_NOSUCCEED;
    pdlt645->head.ctl.ctl.func = CTL_645_RDATA;
    pdlt645->head.len = ( DLT645_TRANSALL_MIN + pData->len);

    pData->di.u32b = DLT645_DI_TRANSMIT;
    dlt645Add33(&pData->di.list[0], sizeof(di645_u));
    memcpy(pdlt645->d, &pData->di.list[0], pdlt645->head.len);

    return DLT645_Compose(pdlt645, buf, bufSize, preCnt);
}

s8 DLT645_downComposeTrans188(DLT645_p pdlt645, dlt645Trans188_p pData, u8* buf, u16* bufSize, u8 preCnt)
{
    if (NULL == pdlt645|| NULL == buf || NULL == bufSize || *bufSize < DLT645_MINSIZE)
        return FALSE;

    pdlt645->head.start1 = DLT645_START_CHAR;
    pdlt645->head.start2 = DLT645_START_CHAR;
    pdlt645->head.ctl.ctl.dir = CTL_645_DOWN;
    pdlt645->head.ctl.ctl.valid = CTL_645_VALID;
    pdlt645->head.ctl.ctl.succeed = CTL_645_NOSUCCEED;
    pdlt645->head.ctl.ctl.func = CTL_645_RDATA;
    pdlt645->head.len = ( DLT645_TRANSALL_MIN + pData->len);

    pData->di.u32b = DLT645_DI_TRANS188;
    dlt645Add33(&pData->di.list[0], sizeof(di645_u));
    memcpy(pdlt645->d, &pData->di.list[0], pdlt645->head.len);

    return DLT645_Compose(pdlt645, buf, bufSize, preCnt);
}
