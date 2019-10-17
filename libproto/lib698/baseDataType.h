#ifndef BASEDATATYPE_H
#define BASEDATATYPE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "libutil/basedef.h"
#include "lib698.h"
#pragma pack(push)
#pragma pack(1)

/*
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
 */

typedef u8 NULL_t;
typedef u8 BOOL_t;
typedef s32 double_long_t;  //-2^31...2^31-1
typedef u32 double_long_unsigned_t; //0...2^32-1
typedef s8 integer_t;   //-128...127
typedef s16 long_t;     //-32768...32767
typedef u8 unsigned_t;  //0...255
typedef u8 long_unsigned_t;  //0...65535
typedef s64 long64_t;  //-2^63...2^63-1
typedef s64 long64_unsigned_t;  //0...2^64-1
typedef u8 ENUM_t;  //0...255
typedef fp32 float32_t; //32位浮点数
typedef fp64 float64_t; //64位浮点数


#pragma pack(pop)

#ifdef __cplusplus
}
#endif

#endif // BASEDATATYPE_H
