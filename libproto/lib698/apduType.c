#include "baseDataType.h"
#include "apduType.h"

#ifdef __cplusplus
extern "C" {
#endif

boolean decodeApdu (u8* apdu, u32 apduLen, link_apdu_p pLinkApdu)
{

}

boolean decodeLinkApdu(u8* apdu, u32 apduLen, link_apdu_p pLinkApdu)
{
    if (NULL == apdu || 0 == apduLen || NULL == pLinkApdu)
        return FALSE;

    if
}


#ifdef __cplusplus
}
#endif
