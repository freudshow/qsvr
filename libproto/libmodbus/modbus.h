#ifndef CCO_HPLC_MODBUS_H
#define CCO_HPLC_MODBUS_H

#include "libutil/basedef.h"

typedef enum {
    e_modbus_ack_data = 0, e_modubs_ack_error
} modbusData_e;

typedef enum {
    e_modbus_frame_type_not_modbus = -1,
    e_modbus_frame_type_down = 0,
    e_modbus_frame_type_ack_normal = 1,
    e_modbus_frame_type_ack_error = 2,
} modbusFrameType_e;

#pragma pack(push)
#pragma pack(1)

//modbus读取命令帧
typedef struct {
    u8 address;		//模块地址号
    u8 func;		//功能码
    u16 start;		//起始寄存器地址
    u16 regCount;	//寄存器数量
    u16 crcSum;		//crc校验
} modbusSendCmd;

#define CCO_MODBUS_DOWN_LEN		sizeof(modbusSendCmd)	//下行读取命令的报文长度

typedef struct {
    u8 address;		//模块地址号
    u8 func;		//功能码
    struct {
        modbusData_e e;
        union {
            u8 error;
            u8 d[253];
        } ud;
    } data;
    u16 crcSum;		//crc校验
} modbusAck;

#define CCO_MODBUS_ACK_MIN_LEN		(5)		//modbus应答报文的最小长度: 1字节地址+1字节功能码+1字节错误码+2字节crc

typedef struct {
    u16 frameStart;
    u16 frameLen;
} modbusSplit_s;

#pragma pack(pop)

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

extern int getCRCFrame(u8 *buf, u16 bufSize, u16 *frameLen);
extern u16 splitModbusFrames(u8 *buf, u16 bufSize, modbusSplit_s *frameList, u16 frameListLen);
extern u16 printModbusFrames(u8 *buf, u16 bufSize);
extern modbusFrameType_e isModbusFrame(u8 *buf, u16 bufSize, u16 *frameLen);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif//CCO_HPLC_MODBUS_H
