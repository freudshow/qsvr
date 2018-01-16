#ifdef __cplusplus
extern "C" {
#endif

#include "stdio.h"
#include "string.h"
#include "lib698.h"
#include "fcs.h"

/*
 * check if this frame is valid
 * @buf: frame buffer
 * @bufSize: frame length
 * return: FALSE-invalid; TRUE-valid.
 */
u8 checkFrame(u8* buf, u16 bufSize, frmHead_s* pFrmhead)
{

    if((NULL == buf) || (0 == bufSize) || (NULL == pFrmhead))
        return FALSE;

    u16 crc = 0;
    u16 fcs = 0;
    u8* p = buf;


    while((FRM_WAKEUP == *p) && (bufSize > 0)) {//skip wake up code
        p++;
        bufSize--;
    }

    if(0 == bufSize)
        return FALSE;

     //check start code and end code
    if((FRM_PREFIX != *p) || (FRM_SUFFIX != p[bufSize-1]))
        return FALSE;

    //check head length
    if(bufSize <= HEAD_LEN_EXCEPT_SA)
        return FALSE;

    memcpy(pFrmhead, p, HEAD_LEN_BEFORE_SA);
    pFrmhead->headLen = HEAD_LEN(pFrmhead->sa.saLen.sa.saLen);
    if(bufSize < pFrmhead->headLen)
        return FALSE;

    memcpy(&(pFrmhead->sa.sa), (p+HEAD_LEN_BEFORE_SA), SA_LEN(pFrmhead->sa.saLen.sa.saLen));

    //check frame length
    if(bufSize != (pFrmhead->frmLen.len.len+2))
        return FALSE;

    //check head's crc
    memcpy(&pFrmhead->headChk,
           (p+HEAD_LEN_BEFORE_SA+SA_LEN(pFrmhead->sa.saLen.sa.saLen)+sizeof(u8)),
           sizeof(u16));

    crc = fcs16((p+1), (pFrmhead->headLen-3));

    if(crc != pFrmhead->headChk)
        return FALSE;

    //check frame's crc
    memcpy(&crc, p+bufSize-3, sizeof(u16));

    fcs = fcs16((p+1), (pFrmhead->frmLen.len.len-2));
    if(crc != fcs)
        return FALSE;

    return TRUE;
}

/*
 * process 698 frame
 * @buf: frame buffer
 * @bufSize: frame length
 * return: FALSE-frame invalid; TRUE-frame valid.
 */
u8 processFrame(u8* buf, u16 bufSize)
{
    if((NULL == buf) || (0 == bufSize))
        return FALSE;

    frmHead_s frmhead = {0};

    if(checkFrame(buf, bufSize, &frmhead) == FALSE) {
        DEBUG_TIME_LINE("frame invalid");
        return FALSE;
    }

    switch (frmhead.ctlChar.dirPrm.dpAssem) {
    case DIR_PRM_CLT_RESPONSE:

        break;
    case DIR_PRM_CLT_REQUEST:

        break;
    case DIR_PRM_SRV_REPORT:

        break;
    case DIR_PRM_SRV_RESPONSE:

        break;
    default:
        break;
    }

    return TRUE;
}

#ifdef __cplusplus
}
#endif
