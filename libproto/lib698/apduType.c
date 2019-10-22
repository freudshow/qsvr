#include "baseDataType.h"
#include "apduType.h"

#ifdef __cplusplus
extern "C" {
#endif


boolean decodeLinkRequestApdu(u8* apdu, u16 apduLen, apdu_p pApdu)
{
    if (NULL == apdu || 0 == apduLen || NULL == pApdu)
        return FALSE;

    u8* pBuf = apdu;

    if (e_link_request_type == *pBuf) {

    }

    return TRUE;
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

    switch (*pBuf) {
    case e_link_request_type:
        decodeLinkRequestApdu(apdu, apduLen, pApdu);
        break;
    case e_link_response_type:
        decodeLinkResponseApdu(apdu, apduLen, pApdu);
        break;
    default:
        return FALSE;
    }

    return TRUE;
}

#ifdef __cplusplus
}
#endif
