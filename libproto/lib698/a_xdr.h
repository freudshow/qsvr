#ifndef A_XDR_H
#define A_XDR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "libutil/basedef.h"

/*!
 * a-xdr base types:
 * integer
 * boolean
 * enumerated
 * bit string
 * byte string
 * choice
 * tags
 * optional, default
 * sequence
 * sequence of
 * VisibleString
 * GeneralizedTime
 * NULL
 */

#pragma pack(push)
#pragma pack(1)

int parseSequenceOf(u8* buf, u32 buflen);
int composeSequenceOf(u8* buf, u32 buflen);


#pragma pack(pop)

#ifdef __cplusplus
}
#endif

#endif // A_XDR_H
