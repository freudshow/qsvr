#include "baseDataType.h"
#include "apduType.h"

#ifdef __cplusplus
extern "C" {
#endif


boolean decodeLinkRequestApdu(u8* buf, u16 bufSize, apdu_p pApdu)
{
    if (NULL == buf || 0 == bufSize || NULL == pApdu)
        return FALSE;

    u8* pBuf = buf;

    if (e_link_request_type == *pBuf) {
        pBuf++;
        pBuf += decode_piid_acd(pBuf, bufSize-(pBuf-buf), &pApdu->u.link_request.piidacd);

        pApdu->u.link_request.type = *pBuf;
        pBuf++;

        pBuf += decode_long_unsigned(pBuf, bufSize-(pBuf-buf), &pApdu->u.link_request.heart);
        pBuf += decode_date_time(pBuf, bufSize-(pBuf-buf), &pApdu->u.link_request.requestTime);

        return TRUE;
    }

    return FALSE;
}

boolean decodeLinkResponseApdu(u8* apdu, u16 apduLen, apdu_p pApdu)
{
    if (NULL == apdu || 0 == apduLen || NULL == pApdu)
        return FALSE;

    u8* pBuf = apdu;

    if (e_link_response_type == *pBuf) {

    }

    return TRUE;
}

boolean decodeLinkApdu(u8* apdu, u16 apduLen, apdu_p pApdu)
{
    if (NULL == apdu || 0 == apduLen || NULL == pApdu)
        return FALSE;

    u8* pBuf = apdu;
    boolean ret = FALSE;

    switch (*pBuf) {
    case e_link_request_type:
        pApdu->e = e_link_request_type;
        ret = decodeLinkRequestApdu(apdu, apduLen, pApdu);
        break;
    case e_link_response_type:
        pApdu->e = e_link_response_type;
        ret = decodeLinkResponseApdu(apdu, apduLen, pApdu);
        break;
    default:
        ret = FALSE;
    }

    return ret;
}

#ifdef __cplusplus
}
#endif
