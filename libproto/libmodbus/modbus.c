#include "modbus.h"
#include <string.h>

/**************************************************
 * 功能描述: 将1个缓冲区内的1条modbus报文长度定位出来
 * ------------------------------------------------
 * 输入参数: buf - 报文缓冲区
 *        bufSize - 报文缓冲区长度
 * 输出参数: pFrameLen - 如果当前缓冲区内存在1帧合法的modbus报文,
 *        则通过此参数返回从缓冲区开始到modbus报文结束的长度
 * ------------------------------------------------
 * 返回值: 成功, 返回 0;
 *       失败, 返回 -1
 * ------------------------------------------------
 * 修改日志:
 * 		1. 日期: 2022年3月29日
 *				创建函数
 **************************************************/
int getCRCFrame(u8 *buf, u16 bufSize, u16 *pFrameLen)
{
    const u16 min = sizeof(u16) + 1;
    if (bufSize < min)
    {
        return -1;
    }

    if (pFrameLen != NULL)
    {
        *pFrameLen = 0;
    }

    u16 len = 0;
    for (len = min; len <= bufSize; len++)
    {
        if (crc16TblDrv(buf, len) == 0)
        {
            if (pFrameLen != NULL)
            {
                *pFrameLen = len;
            }

            return 0;
        }
    }

    return -1;
}

/**************************************************
 * 功能描述: 将1个缓冲区内的多条modbus报文分割开.
 *        前提是, 缓冲区内的报文必须都是合法的modbus, 不能出现
 *        其他多余字符
 * ------------------------------------------------
 * 输入参数: buf - 报文缓冲区
 *        bufSize - 报文缓冲区长度
 * 输出参数: frameList - 存放每条modbus报文的起始位置的列表
 *        frameListLen - 起始位置列表的最大长度
 * ------------------------------------------------
 * 返回值: 成功, 返回 modbus报文个数;
 *       失败, 返回 0
 * ------------------------------------------------
 * 修改日志:
 * 		1. 日期: 2022年3月29日
 *				创建函数
 **************************************************/
u16 splitModbusFrames(u8 *buf, u16 bufSize, modbusSplit_s *frameList,
        u16 frameListLen)
{
    u16 start = 0;
    u16 frameLen = 0;
    u16 idx = 0;

    while (start < bufSize)
    {
        if (getCRCFrame(buf + start, bufSize - start, &frameLen) == 0)
        {
            if (idx < frameListLen)
            {
                frameList[idx].frameStart = start;
                frameList[idx].frameLen = frameLen;

                start += frameLen;
                idx++;
            }
            else
            {
                break;
            }
        }
        else
        {
            break;
        }
    }

    return idx;
}

/**************************************************
 * 功能描述: 如果1个报文缓冲区内有合法的modbus报文, 则定位其起始索引
 * ------------------------------------------------
 * 输入参数: buf - 报文缓冲区
 *        bufSize - 报文缓冲区长度
 * 输出参数: pFrameLen - 如果当前缓冲区内存在1帧合法的modbus报文,
 *        则通过此参数返回从缓冲区开始到modbus报文结束的长度
 * ------------------------------------------------
 * 返回值: 成功, 返回 0;
 *       失败, 返回 -1
 * ------------------------------------------------
 * 修改日志:
 *      1. 日期: 2022年3月29日
 *              创建函数
 **************************************************/
int getModbusStartIdx(u8 *buf, u16 bufSize, u16 *pFrameLen)
{
    int i = 0;

    if (bufSize <= 3)
    {
        return -1;
    }

    for (i = 0; i < bufSize - 3; i++)
    {
        if (getCRCFrame(buf + i, bufSize, pFrameLen) == 0 && (*pFrameLen) > 0)
        {
            return i;
        }
    }

    return -1;
}

/**************************************************
 * 功能描述: 将1个缓冲区内的多条modbus报文分割开.
 *        前提是, 缓冲区内的modbus报文必须都连续排列
 * ------------------------------------------------
 * 输入参数: buf - 报文缓冲区
 *         bufSize - 报文缓冲区长度
 * 输出参数: 无
 * ------------------------------------------------
 * 返回值: 成功, 返回 modbus报文个数;
 *       失败, 返回 0
 * ------------------------------------------------
 * 修改日志:
 *      1. 日期: 2022年3月29日
 *              创建函数
 **************************************************/
u16 printModbusFrames(u8 *buf, u16 bufSize)
{
    u16 len = 0;
    int st = getModbusStartIdx(buf, bufSize, &len);

    if (st <= 0)
    {
        return 0;
    }

    modbusSplit_s frmList[100] = { {0} };

    DEBUG_TIME_LINE("start with: ---> %d", st);
    u16 count = splitModbusFrames(buf + st, bufSize - st, frmList, NELEM(frmList));
    u16 i=0;
    for ( i = 0; i < count; i++)
    {
        DEBUG_BUFF_FORMAT(buf+frmList[i].frameStart, frmList[i].frameLen,
                                "start: %d, frameLen: %d, frame:--->", frmList[i].frameStart, frmList[i].frameLen);
    }

    return count;
}

/**************************************************
 * 功能描述: 检测1帧报文是否含有modbus下行抄读帧.
 * ------------------------------------------------
 * 输入参数: buf - 报文缓冲区
 *        bufSize - 报文缓冲区长度
 * 输出参数: frameLen - 解析出1帧报文的长度
 * ------------------------------------------------
 * 返回值: 成功, 返回 0;
 *       失败, 返回 -1
 * ------------------------------------------------
 * 修改日志:
 * 		1. 日期: 2022年3月29日
 *				创建函数
 **************************************************/
static int isModbusDownRead(u8 *buf, u16 bufSize, u16 *frameLen)
{
    if (bufSize < CCO_MODBUS_DOWN_LEN)
    {
        return -1;
    }

    u16 crc = 0;
    modbusSendCmd cmd = { 0 };
    memcpy(&cmd, buf, CCO_MODBUS_DOWN_LEN);
    crc = crc16TblDrv(buf, CCO_MODBUS_DOWN_LEN - sizeof(crc));
    if (crc != cmd.crcSum)
    {
        return -1;
    }

    *frameLen = CCO_MODBUS_DOWN_LEN;

    return 0;
}

/**
 * 检测1帧报文是否为合法的modbus帧.
 * 注意: 本函数能检测多个modbus帧混合在一起的
 * 报文缓冲区, 可以分割开每帧modbus报文. 但是
 * 有个前提: 所有的modbus帧都是合法的, 且没有
 * 不完整的modbus帧, 没有多余的非法字符.
 */
modbusFrameType_e isModbusFrame(u8 *buf, u16 bufSize, u16 *frameLen)
{
    if (buf == NULL || bufSize < CCO_MODBUS_ACK_MIN_LEN)
    {
        return e_modbus_frame_type_not_modbus;
    }

    if (frameLen != NULL)
    {
        *frameLen = 0;
    }

    u16 crc = 0;

    /**
     * modbus的下行抄读报文为8字节();
     * 上行报文正常应答的长度: 1字节地址域+
     *                  1字节功能码+
     *                  1字节长度+
     *                  2xN字节数据域(N为寄存器数量)+
     *                  2字节crc
     *                  则最终的报文长度为 5 + 2xN, 为奇数,
     *                  故不可能为偶数8, 所以长度为8的crc校验
     *                  正确的modbus报文, 只能是下行抄读报文
     * 上行异常应答报文长度:  1字节地址域+
     *                  1字节功能码+
     *                  1字节异常码+
     *                  2字节crc, 共5字节
     */
    if (isModbusDownRead(buf, bufSize, frameLen) == 0)
    {
        return e_modbus_frame_type_down;
    }
    else if (bufSize == CCO_MODBUS_ACK_MIN_LEN)
    {
        crc = crc16TblDrv(buf, CCO_MODBUS_ACK_MIN_LEN - sizeof(crc));
        u16 crcCode = 0;
        memcpy(&crcCode, &buf[CCO_MODBUS_ACK_MIN_LEN - sizeof(crc)],
                sizeof(crc));

        if (crc != crcCode)
        {
            return e_modbus_frame_type_not_modbus;
        }

        return e_modbus_frame_type_ack_error;
    }
    else
    { //报文长度既不是8, 也不是5, 一定是应答帧
        u16 dataLen = buf[2];
        u16 crcCode = 0;

        if (dataLen != (bufSize - CCO_MODBUS_ACK_MIN_LEN))
        {
            return e_modbus_frame_type_not_modbus;
        }

        memcpy(&crc, &buf[bufSize - sizeof(crc)], sizeof(crc));
        crcCode = crc16TblDrv(buf, bufSize - sizeof(crc));
        if (crc != crcCode)
        {
            return e_modbus_frame_type_not_modbus;
        }

        return e_modbus_frame_type_ack_normal;
    }
}
