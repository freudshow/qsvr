#ifndef	__FCS_H__
#define	__FCS_H__
#include "basedef.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned short	u16;
typedef unsigned char	u8;

extern u16 fcs16(u8* cp, u16 len);

#ifdef __cplusplus
}
#endif

#endif//__FCS_H__
