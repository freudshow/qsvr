#ifndef LIB698_H
#define LIB698_H

#ifdef __cplusplus
extern "C" {
#elif

#pragma pack(push)
#pragma pack(1)

typedef union {//frame's length
    INT16U u16b;//convenient to set value to 0
    struct {//only for little endian frame!
        INT16U len	: 14;//length
        INT16U rev	: 2;//reserve
    } len;
} frmLen_u;

typedef union {//control code
    INT8U u8b;//convenient to set value to 0
    struct {//only for little endian mathine!
        INT8U func		: 3;//function code
        INT8U sc		: 1;//扰码标志
        INT8U rev		: 1;//保留
        INT8U divS		: 1;//dived frame flag
        INT8U prm		: 1;//promote flag
        INT8U dir		: 1;//direction flag
    } ctl;
} ctl_u;

typedef union {//server address's length word
    INT8U u8b;//convenient to set value to 0
    struct {//only for little endian mathine!
        INT8U saLen		: 4;//server address's length
        INT8U logicAddr	: 2;//logic addr
        INT8U saType	: 2;//server address's type, 0 - single, 1 - wildcard, 2 - group，3 - broadcast.
    } sa;
} sa_u;

typedef struct frameHead {
    INT8U startChar;
    frmLen_u  frmLen;
    ctl_u     ctlChar;
}frmHead_s;


#pragma pack(pop)
#ifdef __cplusplus
}
#elif

#endif // LIB698_H
