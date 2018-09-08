#ifdef __cplusplus
extern "C" {
#endif

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "lib698.h"
#include "fcs.h"
#include "linkManager.h"
#include "apduManager.h"

/*
 * check if this frame is valid
 * @buf: frame buffer
 * @bufSize: frame length
 * return: FALSE-invalid; TRUE-valid.
 */
u8 checkFrame(u8* buf, u16* bufSize, frmHead_s* pFrmhead)
{

    if((NULL == buf) || (NULL == bufSize) || (NULL == pFrmhead))
        return FALSE;

    u16 crc = 0;
    u16 fcs = 0;
    u8* p = buf;


    while((FRM_PREFIX != *p) && (*bufSize > 0)) {//find start code
        p++;
        *bufSize--;
    }

    buf = p;

    if(0 == *bufSize)
        return FALSE;

     //check end code
    if(FRM_SUFFIX != p[*bufSize-1])
        return FALSE;

    //check head length
    if(*bufSize <= HEAD_LEN_EXCEPT_SA)
        return FALSE;

    memcpy(pFrmhead, p, HEAD_LEN_BEFORE_SA);

    DEBUG_TIME_LINE("frmLen: %u,\n dir: sent by %s,\n prm: promoted by %s,\n divS: %s apdu,\n func: %s",
                    pFrmhead->frmLen.len.len,
                    (pFrmhead->ctlChar.ctl.dir==0?"client":"server"),
                    (pFrmhead->ctlChar.ctl.prm==0?"server":"client"),
                    (pFrmhead->ctlChar.ctl.divS==0?"whole":"piece of"),
                    (pFrmhead->ctlChar.ctl.func==1?"link managment, logon, heart beat and logout.":(\
                     pFrmhead->ctlChar.ctl.func==3? "apdu managment and data exchange." : "unknown function code" )));

    pFrmhead->headLen = HEAD_LEN(pFrmhead->sa.saLen.sa.saLen);
    DEBUG_TIME_LINE("head's len: %u", pFrmhead->headLen);
    if(*bufSize < pFrmhead->headLen)
        return FALSE;

    pFrmhead->sa.saLen.sa.saLen = SA_LEN(pFrmhead->sa.saLen.sa.saLen);
    DEBUG_TIME_LINE("server's len: %u", pFrmhead->sa.saLen.sa.saLen);
    if(NULL != pFrmhead->sa.sa)
        return FALSE;
    pFrmhead->sa.sa = calloc(1, pFrmhead->sa.saLen.sa.saLen*sizeof(*(pFrmhead->sa.sa)));
    if(NULL == pFrmhead->sa.sa)
        return FALSE;

    memcpy((u8*)(pFrmhead->sa.sa), (p+HEAD_LEN_BEFORE_SA), pFrmhead->sa.saLen.sa.saLen);
    DEBUG_TIME_LINE("server id:");
    printBuf((u8*)pFrmhead->sa.sa, pFrmhead->sa.saLen.sa.saLen, 0, 1);

    //check frame length
    if(*bufSize != (pFrmhead->frmLen.len.len+2))
        return FALSE;

    pFrmhead->ca = *(p+HEAD_LEN_BEFORE_SA+pFrmhead->sa.saLen.sa.saLen);
    DEBUG_TIME_LINE("client id: %02X", pFrmhead->ca);
    //check head's crc
    memcpy(&pFrmhead->headChk,
           (p+HEAD_LEN_BEFORE_SA+pFrmhead->sa.saLen.sa.saLen+sizeof(u8)),
           sizeof(u16));

    crc = fcs16((p+1), (pFrmhead->headLen-3));


    if(crc != pFrmhead->headChk)
        return FALSE;

    //check frame's crc
    memcpy(&crc, p+*bufSize-3, sizeof(u16));

    fcs = fcs16((p+1), (pFrmhead->frmLen.len.len-2));
    if(crc != fcs) {
        return FALSE;
    }
    return TRUE;
}

void scamCode(u8 *buf,int len, boolean addScam)
{
    int i=0;

    if(TRUE == addScam) {
        for(i=0;i<len;i++)
            buf[i] = buf[i] + 0x33;
    } else {
        for(i=0;i<len;i++)
            buf[i] = buf[i] - 0x33;
    }
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

    boolean ret = TRUE;
    frmHead_s frmhead;

    bzero((u8*)&frmhead, sizeof(frmhead));
    ret = checkFrame(buf, &bufSize, &frmhead);
    if( FALSE == ret) {
        DEBUG_TIME_LINE("frame invalid");
        ret = FALSE;
        goto onret;
    }

    switch (frmhead.ctlChar.ctl.func) {
    case FUNC_LINK_MANAGE:
        ret = linkManager(buf, bufSize, &frmhead);
        break;
    case FUNC_LINK_UERDATA:
        ret = userDataManager(buf, bufSize, &frmhead);
        break;
    default:
        break;
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

 onret:
    if(frmhead.sa.sa != NULL)
        free(frmhead.sa.sa);

    return ret;
}

#ifdef __cplusplus
}
#endif
