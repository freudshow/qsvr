#ifndef BASEDATATYPE_H
#define BASEDATATYPE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "libutil/basedef.h"


/******************************************************
 * 通用数据类型的标记与 Data 类型中的
 * CHOICE tag 一致
 * 	Data ::= CHOICE {
 *      null				[0] NULL,
 *      array				[1] SEQUENCE OF Data,
 *      structure			[2] SEQUENCE OF Data,
 *      bool				[3] Bool,
 *      bitString			[4] Bit-string,
 *      doubleLong			[5] DoubleLong,
 *      doubleLongunsigned	[6] DoubleLongUnsigned,
 *      octetString			[9] Octet-string,
 *      visibleString		[10] VisibleString,
 *      utf8String			[12] UTF8String,
 *      integer				[15] Integer,
 *      long				[16] Long,
 *      unsigned			[17] Unsigned,
 *      longunsigned		[18] LongUnsigned,
 *      long64				[20] Long64,
 *      long64Unsigned		[21] Long64Unsigned,
 *      enum				[22] Enum,
 *      float32				[23] Float32,
 *      float64				[24] Float64,
 *      dateTime			[25] Date-Time,
 *      date				[26] Date,
 *      time				[27] Time,
 *      date-Time-s			[28] Date-Time-S,
 *      oi					[80] OI,
 *      oad					[81] OAD,
 *      road				[82] ROAD,
 *      omd					[83] OMD,
 *      ti					[84] TI,
 *      tsa					[85] TSA,
 *      mac					[86] MAC,
 *      rn			     	[87] RN,
 *      region				[88] Region,
 *      scalerUnit			[89] ScalerUnit,
 *      rsd					[90] RSD,
 *      csd					[91] CSD,
 *      ms					[92] MS,
 *      sid					[93] SID,
 *      sidMac 	        	[94] SID-MAC,
 *      comdcb            	[95] COMDCB,
 *      rcsd              	[96] RCSD
 *  }
 ******************************************************/


/*************************************************
 * basic types start
 *************************************************/
typedef u8 NULL_t;
typedef u8 BOOL_t;
typedef s32 double_long_t;  //-2^31...2^31-1
typedef u32 double_long_unsigned_t; //0...2^32-1
typedef s8 integer_t;   //-128...127
typedef s16 long_t;     //-32768...32767
typedef u8 unsigned_t;  //0...255
typedef u16 long_unsigned_t;  //0...65535
typedef s64 long64_t;  //-2^63...2^63-1
typedef u64 long64_unsigned_t;  //0...2^64-1
typedef u8 ENUM_t;  //0...255
typedef fp32 float32_t; //32位浮点数
typedef fp64 float64_t; //64位浮点数
typedef u8 enumerated_t;

typedef unsigned char*   MAC_t; //Message Authentication Code, octet-string
typedef unsigned char*   RN_t;	//random number, octet-string
typedef long_unsigned_t  OI_t;	//object identification


typedef struct asn_struct_ctx_s {
    short phase;		/* Decoding phase */
    short step;		/* Elementary step of a phase */
    int context;		/* Other context information */
    void *ptr;		/* Decoder-specific stuff (stack elements) */
    s32 left;	/* Number of bytes left, -1 for indefinite */
} asn_struct_ctx_t;

typedef struct BIT_STRING_s {
    u8 *buf;	/* BIT STRING body */
    u32 size;	/* Size of the above buffer */
    int bits_unused;/* Unused trailing bits in the last octet (0..7) */
    asn_struct_ctx_t _asn_ctx;	/* Parsing across buffer boundaries */
} BIT_STRING_t;





/*************************************************
 * basic types end
 *************************************************/



/*************************************************
 * sequence types start
 *************************************************/


/*
 *     PIID ::= Unsigned    --PIID 用于客户机 APDU(Client-APDU)的服务数据类型中,  其中:
 *                          --bit7(服务优先级)
 *                          --0: 普通优先级,  1:高优先级, 在应答 APDU
 *                          --中, 其值与请求的 APDU 相同.
 *                          --bit6（保留).
 *                          --bit0..bit5（服务序号): 二进制编码表示 0..63,  在应答 APDU 中,
 *                          --其值与请求的 APDU 相同.
 */
typedef union piid_s {
    u8 u8b;
    struct {
        u8 seq      :6;
        u8 reserve  :1;
        u8 priority :1;
    } piid;
} piid_t;


/*
 *     PIID-ACD ::= Unsigned   --PIID-ACD 用于服务器 APDU(Server-APDU)的服务数据类型中, 其中:
 *                          --bit7(服务优先级): 见 PIID.
 *                          --bit6(请求访问 ACD): 0:不请求, 1:请求。
 *                          --bit0..bit5(服务序号): 见 PIID.
 */
typedef union piid_acd_s {
    u8 u8b;
    struct {
        u8 seq      :6;
        u8 acd      :1;
        u8 priority :1;
    } piid;
} piid_acd_t;


/*
 * year, milliseconds=FFFFH 表示无效.
 * month, day_of_month, day_of_week, hour, minute, second=FFH表示无效.
 * day_of_week: 0 表示周日, 1...6 分别表示周一到周六.
 */
typedef struct date_time_s {
    long_unsigned_t year;
    unsigned_t      month;
    unsigned_t      day_of_month;
    unsigned_t      day_of_week;
    unsigned_t      hour;
    unsigned_t      minute;
    unsigned_t      second;
    long_unsigned_t milliseconds;
}date_time_t;

/*
 * year=FFFFH 表示无效.
 * month, day, hour, minute, second=FFH表示无效.
 */
typedef struct date_timeS_s {
    long_unsigned_t year;
    unsigned_t      month;
    unsigned_t      day;
    unsigned_t      hour;
    unsigned_t      minute;
    unsigned_t      second;
}date_timeS_t;



/*************************************************
 * sequence types end
 *************************************************/




/*************************************************
 * decode and encode functions start
 *************************************************/






/*************************************************
 * decode and encode functions end
 *************************************************/





#ifdef __cplusplus
}
#endif

#endif // BASEDATATYPE_H
