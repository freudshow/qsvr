#include "baseDataType.h"
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum { bigEndian=0, littleEndian=1 } endianess;
#ifndef CPU_LITTLE_ENDIAN
#   define CPU_LITTLE_ENDIAN   (isLittleEndian() == littleEndian)
static int isLittleEndian()
{
    int littleEndian = 1;

    return *(char *)&littleEndian == 1;
}
#endif

u16 decode_null(u8* buf, u16 bufSize, NULL_t* pValue)
{
    if ( NULL == buf || bufSize < sizeof(NULL_t) || NULL == pValue)
        return 0;

    *pValue = 0x00;

    return sizeof(NULL_t);
}

u16 encode_null(u8* buf, u16 bufSize, NULL_t* pValue)
{
    if ( NULL == buf || bufSize < sizeof(NULL_t) || NULL == pValue)
        return 0;

    *buf = 0x00;

    return sizeof(NULL_t);
}

u16 decode_bool(u8* buf, u16 bufSize, BOOL_t* pValue)
{
    if ( NULL == buf || bufSize < sizeof(BOOL_t) || NULL == pValue)
        return 0;

    *pValue = *buf;

    return sizeof(BOOL_t);
}

u16 encode_bool(u8* buf, u16 bufSize, BOOL_t* pValue)
{
    if ( NULL == buf || bufSize < sizeof(BOOL_t) || NULL == pValue)
        return 0;

    *buf = *pValue;

    return sizeof(BOOL_t);
}

u16 decode_unsigned(u8* buf, u16 bufSize, unsigned_t* pValue)
{
    if ( NULL == buf || bufSize < sizeof(unsigned_t) || NULL == pValue)
        return 0;

    *pValue = *buf;

    return sizeof(unsigned_t);
}

u16 encode_unsigned(u8* buf, u16 bufSize, unsigned_t* pValue)
{
    if ( NULL == buf || bufSize < sizeof(unsigned_t) || NULL == pValue)
        return 0;

    *buf = *pValue;

    return sizeof(unsigned_t);
}

u16 decode_long_unsigned(u8* buf, u16 bufSize, long_unsigned_t* pValue)
{
    if ( NULL == buf || bufSize < sizeof(long_unsigned_t) || NULL == pValue)
        return 0;

    memcpy(pValue, buf, sizeof(long_unsigned_t));

    /*
     * 698 apdu frame is big endian, so if cpu is
     * little endian, you need to inverse the
     * value
     */
    if (CPU_LITTLE_ENDIAN)
        inverseArray((u8*)pValue, sizeof(long_unsigned_t));

    return sizeof(long_unsigned_t);
}

u16 encode_long_unsigned(u8* buf, u16 bufSize, long_unsigned_t* pValue)
{
    if ( NULL == buf || bufSize < sizeof(long_unsigned_t) || NULL == pValue)
        return 0;

    /*
     * 698 apdu frame is big endian, so if cpu is
     * little endian, you need to inverse the
     * value
     */
    if (CPU_LITTLE_ENDIAN)
        inverseArray((u8*)pValue, sizeof(long_unsigned_t));

    memcpy(buf, pValue, sizeof(long_unsigned_t));

    return sizeof(long_unsigned_t);
}

u16 decode_piid_acd(u8* buf, u16 bufSize, piid_acd_t* pValue)
{
    if ( NULL == buf || bufSize < sizeof(piid_acd_t) || NULL == pValue)
        return 0;

    pValue->u8b = *buf;

    return sizeof(piid_acd_t);
}

u16 encode_piid_acd(u8* buf, u16 bufSize, piid_acd_t* pValue)
{
    if ( NULL == buf || bufSize < sizeof(piid_acd_t) || NULL == pValue)
        return 0;

    *buf = pValue->u8b;

    return sizeof(piid_acd_t);
}

u16 decode_date_time(u8* buf, u16 bufSize, date_time_t* pValue)
{
    if ( NULL == buf || bufSize < DATE_TIME_SIZE || NULL == pValue)
        return 0;

    u8* pBuf = buf;

    pBuf += decode_long_unsigned(pBuf, bufSize-(pBuf-buf), &pValue->year);
    pBuf += decode_unsigned(pBuf, bufSize-(pBuf-buf), &pValue->month);
    pBuf += decode_unsigned(pBuf, bufSize-(pBuf-buf), &pValue->day_of_month);
    pBuf += decode_unsigned(pBuf, bufSize-(pBuf-buf), &pValue->day_of_week);
    pBuf += decode_unsigned(pBuf, bufSize-(pBuf-buf), &pValue->hour);
    pBuf += decode_unsigned(pBuf, bufSize-(pBuf-buf), &pValue->minute);
    pBuf += decode_unsigned(pBuf, bufSize-(pBuf-buf), &pValue->second);
    pBuf += decode_long_unsigned(pBuf, bufSize-(pBuf-buf), &pValue->milliseconds);

    return DATE_TIME_SIZE;
}

u16 encode_date_time(u8* buf, u16 bufSize, date_time_t* pValue)
{
    if ( NULL == buf || bufSize < DATE_TIME_SIZE || NULL == pValue)
        return 0;

    u8* pBuf = buf;

    pBuf += encode_long_unsigned(pBuf, bufSize, &pValue->year);
    pBuf += encode_unsigned(pBuf, bufSize-(pBuf-buf), &pValue->month);
    pBuf += encode_unsigned(pBuf, bufSize-(pBuf-buf), &pValue->day_of_month);
    pBuf += encode_unsigned(pBuf, bufSize-(pBuf-buf), &pValue->day_of_week);
    pBuf += encode_unsigned(pBuf, bufSize-(pBuf-buf), &pValue->hour);
    pBuf += encode_unsigned(pBuf, bufSize-(pBuf-buf), &pValue->minute);
    pBuf += encode_unsigned(pBuf, bufSize-(pBuf-buf), &pValue->second);
    pBuf += encode_long_unsigned(pBuf, bufSize-(pBuf-buf), &pValue->milliseconds);

    return DATE_TIME_SIZE;
}

#ifdef __cplusplus
}
#endif
