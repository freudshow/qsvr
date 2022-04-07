#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include "libcj188.h"

int processCj188()
{
	return 0;
}

s8 cj188Recognize()
{
    return 0;
}

void printAddress(logicAddr_s addr)
{
    fprintf(stderr,
            "vendor number: %04X, logic address: %02X%02X%02X%02X%02X\n",
            addr.laddr.vendor, addr.laddr.b[4], addr.laddr.b[3],
            addr.laddr.b[2], addr.laddr.b[1], addr.laddr.b[0]);
}

void printCtl(ctl188_u ctl)
{
    fprintf(stderr, "direction: %d, valid: %d, \n", ctl.ctl.dir, ctl.ctl.valid);
    switch (ctl.ctl.func) {
        case CTL_RSV:
            fprintf(stderr, "ctr: %d, reserve\n", ctl.ctl.func);
            break;
        case CTL_R_DATA:
            fprintf(stderr, "ctr: %d, read data\n", ctl.ctl.func);
            break;
        case CTL_W_DATA:
            fprintf(stderr, "ctr: %d, write data\n", ctl.ctl.func);
            break;
        case CTL_R_KEYVER:
            fprintf(stderr, "ctr: %d, key version\n", ctl.ctl.func);
            break;
        case CTL_R_ADDR:
            fprintf(stderr, "ctr: %d, read address\n", ctl.ctl.func);
            break;
        case CTL_W_ADDR:
            fprintf(stderr, "ctr: %d, write address\n", ctl.ctl.func);
            break;
        case CTL_W_ELEC_SYNC_DATA:
            fprintf(stderr, "ctr: %d, write electric synchronization data\n",
                    ctl.ctl.func);
            break;
        default:
            fprintf(stderr, "ctr: %d, unknown function code or vendor defined\n",
                    ctl.ctl.func);
            break;
    }
}

/*
 * CJ/T188 报文内容初步解包
 */
s16 cj188unpack(cj188_p pCj188, u8* rcvbuff, u16 rcvlen)
{
    u32 i = 0;
    s16 ret = CHK_SUC;
    u8* p = rcvbuff;

    if (NULL == pCj188 || NULL == rcvbuff || 0 == rcvlen)
        return ERR_RCVD_LOST;

    while(*p != CJ188_START_CHAR && i < rcvlen) {
        p++;    //找报文头
        i++;
    }
    rcvlen -= i;
    ret = firstChk(p, rcvlen);
    if (ret == CHK_SUC) {
        memcpy(&(pCj188->head), p, sizeof(cj188Head_s));
        p += sizeof(cj188Head_s);

        printAddress(pCj188->head.addr);
        printCtl(pCj188->head.ctl);

        if (pCj188->head.len < 3) {//2字节DI, 1字节seq
            ret = ERR_RCVD_LOST;
        } else {
            memcpy(&(pCj188->data.DI), p, 3);//copy di and seq
            p += 3;
            memcpy(&pCj188->data.data[0], p, pCj188->head.len);
        }
    }

    return ret;
}

/***************************************
 * CJ/T188 应用层数据内容解析
 ***************************************/
s16 cj188_parseData(cj188_p pCj188, meterData_p pData)
{
    s16 ret = CHK_SUC;

    switch (pCj188->head.type) {
        case T_WATER_COOL:
        case T_WATER_HEAT:
        case T_WATER_MIDDLE:
            pData->e = e_waterData;
            memset(&(pData->u.wData), 0, sizeof(waterData_s));
            cj188_parseWaterData(pCj188, &(pData->u.wData));
            break;
        case T_HOT_HEAT:
        case T_HOT_COOL:
            pData->e = e_hotData;
            memset(&(pData->u.hData), 0, sizeof(hotData_s));
            cj188_parseHotData(pCj188, &(pData->u.hData));
            break;
        case T_GAS:
            pData->e = e_gasData;
            memset(&(pData->u.gData), 0, sizeof(gasData_s));
            cj188_parseGasData(pCj188, &(pData->u.gData));
            break;
        case T_METER:
            break;
        default:
            fprintf(stderr, "仪表类型不支持\n");
            ret = ERR_METERTYPE;
            break;
    }

    return ret;
}
/*
 * 获取仪表数据宏定义
 * @pCj188: 指向cj188_s结构体的指针
 * @pdata: 指向水气热等数据类型的指针
 * @etype: 水气热等数据类型的枚举
 * @type: 水气热等数据类型的具体数据类型名
 * @member: 水气热等数据类型的具体数据类型的成员名
 * @ret: 返回值
 */
#define GET_METER_DATA(pCj188, pdata, etype, type, member, ret)    do{\
                                                                        if(pCj188->head.len != sizeof(type)) {\
                                                                            ret = ERR_DATA_FMT;\
                                                                        } else{\
                                                                            pdata->e = etype;\
                                                                            memcpy(&(pdata->u.member), &(pCj188->data), sizeof(type));\
                                                                        }\
                                                                    }while(0)

/*
 * 获取水表的上行数据区数据.
 * 外部过程读取水表的数据时,
 * 首先应根据pdata->e的类型
 * 来决定使用哪种数据类型.
 */
s8 getWaterData(cj188_p pCj188, waterData_p pdata)
{
    if (NULL == pCj188 || NULL == pdata)
        return ERR_RCVD_LOST;

    s8 ret = CHK_SUC;
    memset(pdata, 0, sizeof(waterData_s));

    switch (pCj188->data.DI) {
        case DI_MEASURE:
            GET_METER_DATA(pCj188, pdata, e_readDataW, readData1019_s, rdata, ret);
            break;
        case DI_HISTORY_0:
        case DI_HISTORY_1:
        case DI_HISTORY_2:
        case DI_HISTORY_3:
        case DI_HISTORY_4:
        case DI_HISTORY_5:
        case DI_HISTORY_6:
        case DI_HISTORY_7:
        case DI_HISTORY_8:
        case DI_HISTORY_9:
        case DI_HISTORY_A:
        case DI_HISTORY_B:      //历史数据的结构是一样的, 统一处理
            GET_METER_DATA(pCj188, pdata, e_hisDataW, hisData10193049_s, hdata,
                           ret);
            break;
        case DI_PRICE_TBL:
            GET_METER_DATA(pCj188, pdata, e_priceTblW, priceTbl_s, priceTbl, ret);
            break;
        case DI_R_SETTLE_DATE:
            GET_METER_DATA(pCj188, pdata, e_sdateW, sDate_s, sdate, ret);
            break;
        case DI_R_READMETER_DATE:
            GET_METER_DATA(pCj188, pdata, e_rdateW, rDate_s, rdate, ret);
            break;
        case DI_R_CASH:
            break;
        case DI_R_KEYVER:
            break;
        case DI_R_ADDR:
            break;
        case DI_W_PRICE_TBL:
            break;
        case DI_W_SETTLE_DATE:
            break;
        case DI_W_READMETER_DATE:
            break;
        case DI_W_CASH:
            break;
        case DI_W_NEWKEY:
            break;
        case DI_W_STD_TIME:
            break;
        case DI_W_VALVE_CTL:
            break;
        case DI_W_VEDOR_ENABLE:
            break;
        case DI_W_ADDR:
            break;
        case DI_W_ELEC_SYNC_DATA:
            break;
        default:
            ret = ERR_DI_NDEF;
            break;
    }

    return ret;
}

s8 cj188_parseWaterData(cj188_p pCj188, waterData_p pdata)
{
    if(NULL == pCj188 || NULL == pdata)
        return ERR_RCVD_LOST;

    s8 ret = CHK_SUC;
    switch (pCj188->head.ctl.ctl.dir) {
        case CJ188_DOWNFRM:
            break;
        case CJ188_UPFRM:
            switch (pCj188->head.ctl.ctl.valid) {
                case CJ188_ASW_VALID:
                    switch (pCj188->head.ctl.ctl.func) {
                        case CTL_RSV:
                            break;
                        case CTL_R_DATA:
                            ret = getWaterData(pCj188, pdata);
                            break;
                        case CTL_W_DATA:
                            break;
                        case CTL_R_KEYVER:
                            break;
                        case CTL_R_ADDR:
                            break;
                        case CTL_W_ADDR:
                            break;
                        case CTL_W_ELEC_SYNC_DATA:
                            break;
                        default:
                            ret = ERR_CTL_FUNC_NDEF;
                            break;
                    }
                    break;
                case CJ188_ASW_INVALID:
                    GET_METER_DATA(pCj188, pdata, e_slaveInvalidW, slaveInvalid_s,
                                   invalid, ret);
                    break;
            }
            break;
        default:
            break;
    }

    return ret;
}

/*
 * 获取热表的上行数据区数据.
 * 外部过程读取水表的数据时,
 * 首先应根据pdata->e的类型
 * 来决定使用哪种数据类型.
 */
s8 getHotData(cj188_p pCj188, hotData_p pdata)
{
    if (NULL == pCj188 || NULL == pdata)
        return ERR_RCVD_LOST;

    s8 ret = CHK_SUC;
    memset(pdata, 0, sizeof(hotData_s));

    switch (pCj188->data.DI) {
        case DI_MEASURE:
            GET_METER_DATA(pCj188, pdata, e_readDataH, readData2029_s, rdata, ret);
            break;
        case DI_HISTORY_0:
        case DI_HISTORY_1:
        case DI_HISTORY_2:
        case DI_HISTORY_3:
        case DI_HISTORY_4:
        case DI_HISTORY_5:
        case DI_HISTORY_6:
        case DI_HISTORY_7:
        case DI_HISTORY_8:
        case DI_HISTORY_9:
        case DI_HISTORY_A:
        case DI_HISTORY_B:      //历史数据的结构是一样的, 统一处理
            GET_METER_DATA(pCj188, pdata, e_hisDataH, hisData2029_s, hdata,
                           ret);
            break;
        case DI_PRICE_TBL:
            GET_METER_DATA(pCj188, pdata, e_priceTblH, priceTbl_s, priceTbl, ret);
            break;
        case DI_R_SETTLE_DATE:
            GET_METER_DATA(pCj188, pdata, e_sdateH, sDate_s, sdate, ret);
            break;
        case DI_R_READMETER_DATE:
            GET_METER_DATA(pCj188, pdata, e_rdateH, rDate_s, rdate, ret);
            break;
        case DI_R_CASH:
            break;
        case DI_R_KEYVER:
            break;
        case DI_R_ADDR:
            break;
        case DI_W_PRICE_TBL:
            break;
        case DI_W_SETTLE_DATE:
            break;
        case DI_W_READMETER_DATE:
            break;
        case DI_W_CASH:
            break;
        case DI_W_NEWKEY:
            break;
        case DI_W_STD_TIME:
            break;
        case DI_W_VALVE_CTL:
            break;
        case DI_W_VEDOR_ENABLE:
            break;
        case DI_W_ADDR:
            break;
        case DI_W_ELEC_SYNC_DATA:
            break;
        default:
            ret = ERR_DI_NDEF;
            break;
    }

    return ret;
}

s8 cj188_parseHotData(cj188_p pCj188, hotData_p pdata)
{
    if(NULL == pCj188 || NULL == pdata)
        return ERR_RCVD_LOST;

    s8 ret = CHK_SUC;
    switch (pCj188->head.ctl.ctl.dir) {
        case CJ188_DOWNFRM:
            break;
        case CJ188_UPFRM:
            switch(pCj188->head.ctl.ctl.valid) {
                case CJ188_ASW_VALID:
                    switch(pCj188->head.ctl.ctl.func) {
                        case CTL_RSV:
                            break;
                        case CTL_R_DATA:
                            ret = getHotData(pCj188, pdata);
                            break;
                        case CTL_W_DATA:
                            break;
                        case CTL_R_KEYVER:
                            break;
                        case CTL_R_ADDR:
                            break;
                        case CTL_W_ADDR:
                            break;
                        case CTL_W_ELEC_SYNC_DATA:
                            break;
                        default:
                            ret = ERR_CTL_FUNC_NDEF;
                            break;
                    }
                    break;
                case CJ188_ASW_INVALID:
                    GET_METER_DATA(pCj188, pdata, e_slaveInvalidH, slaveInvalid_s,
                                   invalid, ret);
                    break;
            }
            break;
        default:
            break;
    }

    return ret;
}

/*
 * 获取燃气表的上行数据区数据.
 * 外部过程读取燃气表的数据时,
 * 首先应根据pdata->e的类型
 * 来决定使用哪种数据类型.
 */
s8 getGasData(cj188_p pCj188, gasData_p pdata)
{
    if (NULL == pCj188 || NULL == pdata)
        return ERR_RCVD_LOST;

    s8 ret = CHK_SUC;
    memset(pdata, 0, sizeof(gasData_s));

    switch (pCj188->data.DI) {
        case DI_MEASURE:
            GET_METER_DATA(pCj188, pdata, e_readDataG, readData1019_s, rdata, ret);
            break;
        case DI_HISTORY_0:
        case DI_HISTORY_1:
        case DI_HISTORY_2:
        case DI_HISTORY_3:
        case DI_HISTORY_4:
        case DI_HISTORY_5:
        case DI_HISTORY_6:
        case DI_HISTORY_7:
        case DI_HISTORY_8:
        case DI_HISTORY_9:
        case DI_HISTORY_A:
        case DI_HISTORY_B:      //历史数据的结构是一样的, 统一处理
            GET_METER_DATA(pCj188, pdata, e_hisDataG, hisData10193049_s, hdata,
                           ret);
            break;
        case DI_PRICE_TBL:
            GET_METER_DATA(pCj188, pdata, e_priceTblG, priceTbl_s, priceTbl, ret);
            break;
        case DI_R_SETTLE_DATE:
            GET_METER_DATA(pCj188, pdata, e_sdateG, sDate_s, sdate, ret);
            break;
        case DI_R_READMETER_DATE:
            GET_METER_DATA(pCj188, pdata, e_rdateG, rDate_s, rdate, ret);
            break;
        case DI_R_CASH:
            break;
        case DI_R_KEYVER:
            break;
        case DI_R_ADDR:
            break;
        case DI_W_PRICE_TBL:
            break;
        case DI_W_SETTLE_DATE:
            break;
        case DI_W_READMETER_DATE:
            break;
        case DI_W_CASH:
            break;
        case DI_W_NEWKEY:
            break;
        case DI_W_STD_TIME:
            break;
        case DI_W_VALVE_CTL:
            break;
        case DI_W_VEDOR_ENABLE:
            break;
        case DI_W_ADDR:
            break;
        case DI_W_ELEC_SYNC_DATA:
            break;
        default:
            ret = ERR_DI_NDEF;
            break;
    }

    return ret;
}

s8 cj188_parseGasData(cj188_p pCj188, gasData_p pdata)
{
    if(NULL == pCj188 || NULL == pdata)
        return ERR_RCVD_LOST;

    s8 ret = CHK_SUC;
    switch (pCj188->head.ctl.ctl.dir) {
        case CJ188_DOWNFRM:
            break;
        case CJ188_UPFRM:
            switch (pCj188->head.ctl.ctl.valid) {
                case CJ188_ASW_VALID:
                    switch (pCj188->head.ctl.ctl.func) {
                        case CTL_RSV:
                            break;
                        case CTL_R_DATA:
                            ret = getGasData(pCj188, pdata);
                            break;
                        case CTL_W_DATA:
                            break;
                        case CTL_R_KEYVER:
                            break;
                        case CTL_R_ADDR:
                            break;
                        case CTL_W_ADDR:
                            break;
                        case CTL_W_ELEC_SYNC_DATA:
                            break;
                        default:
                            ret = ERR_CTL_FUNC_NDEF;
                            break;
                    }
                    break;
                case CJ188_ASW_INVALID:
                    GET_METER_DATA(pCj188, pdata, e_slaveInvalidG, slaveInvalid_s,
                                   invalid, ret);
                    break;
            }
            break;
        default:
            break;
    }

    return ret;
}

/***************************************
 * CJ/T188 报文检查, 检查以下内容:
 * 1. 检查bufSize
 * 2. 检查开始符0x68
 * 3. 检查报文长度是否与报文内声称的长度相等
 * 4. 检查结束符
 * 5. 检查校验值
 ***************************************/
s16 firstChk(u8* buf, u16 bufSize)
{
    if (bufSize < CJ188_MINSIZE)
        return ERR_RCVD_LOST;
    if (buf[0] != CJ188_START_CHAR)
        return ERR_LOST_0x68;
    if (bufSize != (buf[10] + CJ188_MINSIZE))
        return ERR_RCVD_LOST;
    if (buf[bufSize - 1] != CJ188_END_CHAR)
        return ERR_LOST_0x16;
    if (checkSum(&buf[0], bufSize - 2) != buf[bufSize - 2])
        return ERR_SUM_ERROR;   // 校验失败

    return CHK_SUC;
}

/***************************************
 * CJ/T188 帧校验和
 * 因为CJ/T 188协议里规定的数据区长度,
 * 使用1个字节来表示, 所以一条CJ/T 188
 * 报文长度不可能长于256+13 = 269, 所以
 * 使用u16足够.
 * 0x68-1, 仪表类型-1, 地址域-7,
 * 控制码-1, 长度-1, 校验-1, 0x16-1.
 ***************************************/
u8 checkSum(u8* buf, u16 len)
{
    u16 i = 0;
    u8 ret = 0;

    if (NULL == buf || len == 0)
        return ret;

    for (i = 0; i < len; i++)
        ret += buf[i];

    return ret;
}


static unitStr_s unitSet[] = {
    {UNIT_Wh, "Wh"}, {UNIT_kWh, "kWh"}, {UNIT_MWh, "MWh"},
    {UNIT_MWhX100, "MWhx100"}, {UNIT_J, "J"}, {UNIT_kJ, "kJ"}, {UNIT_MJ, "MJ"},
    {UNIT_GJ, "GJ"}, {UNIT_GJX100, "GJx100"}, {UNIT_W, "W"}, {UNIT_kW, "kW"},
    {UNIT_MW, "MW"}, {UNIT_L, "L"}, {UNIT_M3, "m³"}, {UNIT_Lh, "L/h"},
    {UNIT_M3h, "m³/h"}
};

void getUnitStr(u8 c, char* unit)
{
    if (NULL == unit)
        return;

    u8 len = NELEM(unitSet);
    u8 found = 0;
    u8 i = 0;

    for (i = 0; i < len; i++) {
        if (c == unitSet[i].c) {
            found = 1;
            strcpy(unit, (char*)&(unitSet[i].u[0]));
        }
    }

    if (0 == found)
        strcpy(unit, "unknown");
}

void reversebuf(u8* b, u16 len)
{
    if(NULL == b || 0 == len)
        return;

    u16 i = 0;

    for (i = 0; i< (len/2); i++) {
        SWAP(b[i], b[len-1-i]);
    }
}

/*
 * 将压缩BCD码表示的十进制定点数转为浮点数
 * 由于cj188协议中用到的定点数,
 * 取值范围从0.0001到999999.99,
 * 所以使用float类型就完全足够.
 * @buf: 定点数缓冲区
 * @len: buf的字节数,
 *       由于浮点数的尾数精度有限,
 *       经测试, 在有效数字大于7后,得到的
 *       浮点数误差较大, 故本程序只处理7字节
 *       以下的BCD码.
 * @pos: 小数点的位置,
 *        从整数部分的最低位的右边第一位开始算起,
 *        如563.23的小数点位置为2,
 *          23.00的小数点位置为0.
 * @order: buf中数位的大小端,
 *          0-低位在前, 高位在后
 *          1-高位在前, 低位在后
 * @sign: 是否有符号位, 0-无符号位
 *        1-有符号位, 且buf的最高字节的最高位为符号位
 *                  符号位: 0-正数, 1-负数
 * @return: 返回转换后的浮点数,
 *          如果出错, 返回0.0
 */
float fixP2floatP(u8* buf, u16 len, u8 pos, u8 order, u8 sign)
{
    float fl = 0;
    u8 i = 0;
    u8 s = 0;

    if(0 == len || len > 8 || NULL == buf)
        return 0.0;

    if (1 == order)
        reversebuf(buf, len);

    if (1 == sign) {
        s = BIT_N(buf[len-1], 7);//获取符号位
        buf[len-1] &= (~(0x01 << 7));//清除符号位
    } else if (sign > 1) {
        fprintf(stderr, "sign is greater than 1.\n");
        return 0.0;
    }

    for (i=0; i<len; i++)
        fl += pow(10.0, 2*i)*BCD_TO_HEX(buf[i]);

    fl /= pow(10.0, pos);

    if(s)
        fl *= (-1);

    return fl;
}

/*
 * 参数意义同上
 * 需要注意的是, 有符号long long
 * 类型的最大值的有效数字是18位,
 * 所以本程序只处理长度为8字节
 * 以下的bcd码.
 */
s64 fixP2s64(u8* buf, u16 len, u8 pos, u8 order, u8 sign)
{
    s64 fl = 0;
    u8 i = 0;
    u8 s = 0;

    if(len > 8 || 0 == len || NULL == buf)
        return 0;

    NOT_USED(pos);
    if (1 == order)
        reversebuf(buf, len);

    if (1 == sign) {
        s = BIT_N(buf[len-1], 7);
        buf[len-1] &= (~(0x01 << 7));
    }

    for (i=0; i<len; i++)
        fl += pow(10, 2*i)*BCD_TO_HEX(buf[i]);

    if(s)
        fl *= (-1);

    return fl;
}

/****************************************组帧start*************************************************/
s8 cj188compose(cj188_p pCj188, u8* buf, u16* bufSize, u8 preCnt)
{
    if(NULL == pCj188 || NULL == buf || NULL == bufSize || *bufSize < CJ188_MINSIZE)
        return FALSE;

    u8* p = buf;
    u16 bufLen =  0;

    //前导符目前遇到最长的有13个, 取整到20
    if(preCnt < 20) {
        memset(p, CJ188_PREFIX, preCnt);
        p += preCnt;
    } else {
        return FALSE;
    }

    //头部
    memcpy(p, &pCj188->head, sizeof(cj188Head_s));
    p += sizeof(cj188Head_s);
    //数据域
    memcpy(p, &pCj188->data, pCj188->head.len);
    p += pCj188->head.len;
    //校验和, 不带前导符
    *p = checkSum(buf+preCnt, (p-buf)-preCnt);
    p++;
    //结束符
    *p = CJ188_END_CHAR;
    p++;
    //返回长度

    /*
     * 如果外部调用函数传进来的bufSize, 是作为
     * 某一结构体的成员, 且这个结构体是按照
     * 1字节对齐, 或者其他的非4字节对齐方式,
     * 因为ARM9处理器是按照4字节对齐(具体对齐
     * 方式跟编译器和芯片体系结构都有关系,
     * 需进一步查证), 而函数调用时, 参数列表
     * 也按照4字节对齐, 所以在使用 *bufSize
     * 表达式取得或者设置 bufSize指向的内存
     * 地址的值时, 就会出现字节偏移, 但是在
     * 上位机的x86_64系统下测试, 是没有问题的.
     */
    bufLen = (p-buf);
    memcpy(bufSize, &bufLen, sizeof(u16));
    DEBUG_TIME_LINE("bufSize: %p, %d", bufSize, *bufSize);
    return TRUE;
}


/*****************************组下行读取帧start*******************************/

/*
 * 获取下行读取计量数据报文
 * 外部调用函数须配置仪表类型和仪表地址域
 * @pCj188: cj188_s结构体指针
 * @buf: 发送缓冲区
 * @bufSize: 有两个作用:
 *           1. 传进来时, 指明发送缓冲区的长度,
 *              组帧函数要保证有足够的空间装载帧.
 *           2. 作为返回值, 返回完整报文的长度.
 * @preCnt: 前导符个数
 * @return: 成功-TRUE, 失败-FALSE
 */
s8 cj188ReadData(cj188_p pCj188, u8* buf, u16* bufSize, u8 preCnt)
{
    DEBUG_TIME_LINE("bufSize: %p, %d", bufSize, *bufSize);
    if(NULL == pCj188 || NULL == buf || NULL == bufSize || *bufSize < CJ188_MINSIZE)
        return FALSE;


    //头部
    pCj188->head.start = CJ188_START_CHAR;
    pCj188->head.ctl.ctl.dir = CJ188_DOWNFRM;
    pCj188->head.ctl.ctl.valid = CJ188_ASW_VALID;
    pCj188->head.ctl.ctl.func = CTL_R_DATA;
    pCj188->head.len = 3;

    //数据域
    pCj188->data.DI = DI_MEASURE;

    return cj188compose(pCj188, buf, bufSize, preCnt);
}

/*
 * 获取上行读取计量数据报文
 * 外部调用函数须配置仪表类型和仪表地址域
 * @pCj188: cj188_s结构体指针
 * @buf: 发送缓冲区
 * @bufSize: 有两个作用:
 *           1. 传进来时, 指明发送缓冲区的长度,
 *              组帧函数要保证有足够的空间装载帧.
 *           2. 作为返回值, 返回完整报文的长度.
 * @preCnt: 前导符个数
 * @return: 成功-TRUE, 失败-FALSE
 */
s8 cj188UpReadData(cj188_p pCj188, meterData_p pData, u8* buf, u16* bufSize, u8 preCnt)
{
    s8 ret = TRUE;
    u16 len = 0;

    memcpy(&len, bufSize, sizeof(u16));
    if(NULL == pCj188 || NULL == buf || NULL == bufSize || len < CJ188_MINSIZE)
        return FALSE;

    //头部
    pCj188->head.start = CJ188_START_CHAR;
    pCj188->head.ctl.ctl.dir = CJ188_UPFRM;
    pCj188->head.ctl.ctl.valid = CJ188_ASW_VALID;
    pCj188->head.ctl.ctl.func = CTL_R_DATA;

    //数据域
    switch(pData->e) {
        case e_waterData:
            if(pData->u.wData.e == e_readDataW) {
                pCj188->head.len = sizeof(readData1019_s);
                memcpy(pCj188->data.data, &pData->u.wData.u.rdata, sizeof(readData1019_s));
            }
            break;
        case e_hotData:
            if(pData->u.hData.e == e_readDataH) {
                pCj188->head.len = sizeof(readData2029_s);
                memcpy(pCj188->data.data, &pData->u.hData.u.rdata, sizeof(readData2029_s));
            }
            break;
        case e_gasData:
            if(pData->u.gData.e == e_readDataG) {
                pCj188->head.len = sizeof(readData1019_s);
                memcpy(pCj188->data.data, &pData->u.gData.u.rdata, sizeof(readData1019_s));
            }
            break;
    }

    ret = cj188compose(pCj188, buf, &len, preCnt);
    if(ret != TRUE)
        len = 0;
    memcpy(bufSize, &len, sizeof(u16));

    return ret;
}



/*****************************组下行读取帧end*******************************/



/*****************************组下行写入帧start*******************************/

/*****************************组下行写入帧end*******************************/

/****************************************组帧end*************************************************/
