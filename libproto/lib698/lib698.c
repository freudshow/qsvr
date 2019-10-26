#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib698.h"
#include "linkManager.h"
#include "apduManager.h"


void scamCode(u8 *buf, u16 len, boolean addScam)
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
 * check if this frame is valid
 * @buf: frame buffer
 * @bufSize: frame length
 * return: FALSE-invalid; TRUE-valid.
 */
boolean checkFrame(u8* buf, u16* bufSize, frmHead_s* pFrmhead)
{
    if((NULL == buf) || (NULL == bufSize) || (NULL == pFrmhead))
        return FALSE;

    u16 crc = 0;
    u16 fcs = 0;
    u8* p = buf;

    //find start code
    while((DLT69845_START_CHAR != *p) && (*bufSize > 0)) {
        p++;
        (*bufSize)--;
    }

    buf = p;

    if(0 == *bufSize)
        return FALSE;

     //check end code
    if(DLT69845_END_CHAR != p[*bufSize-1])
        return FALSE;

    //check head length
    if(*bufSize <= DLT69845_HEAD_LEN_EXCEPT_SA)
        return FALSE;

    memcpy(pFrmhead, p, DLT69845_HEAD_LEN_BEFORE_SA);

    DEBUG_TIME_LINE("frmLen: %u,\n dir: sent by %s,\n prm: promoted by %s,\n divS: %s apdu,\n ScramblingCode: %s,\n func: %s",
                    pFrmhead->frmLen.len.len,
                    (pFrmhead->ctlChar.ctl.dir==0?"client":"server"),
                    (pFrmhead->ctlChar.ctl.prm==0?"server":"client"),
                    (pFrmhead->ctlChar.ctl.divS==0?"whole":"piece of"),
                    (pFrmhead->ctlChar.ctl.sc == 1?"ScramblingCode added ":"ScramblingCode not added"),
                    (pFrmhead->ctlChar.ctl.func==1?"link managment, logon, heart beat and logout.":(\
                     pFrmhead->ctlChar.ctl.func==3? "apdu managment and data exchange." : "unknown function code" )));

    pFrmhead->headLen = DLT69845_HEAD_LEN(pFrmhead->sa.saLen.sa.saLen);
    DEBUG_TIME_LINE("head's len: %u", pFrmhead->headLen);
    if(*bufSize < pFrmhead->headLen)
        return FALSE;

    pFrmhead->apduLen = pFrmhead->frmLen.len.len - pFrmhead->headLen - 1;
    pFrmhead->sa.saLen.sa.saLen = DLT69845_SA_LEN(pFrmhead->sa.saLen.sa.saLen);
    DEBUG_TIME_LINE("server's len: %u, logicAddr: %02X, ", pFrmhead->sa.saLen.sa.saLen,
    		pFrmhead->sa.saLen.sa.logicAddr);

	switch (pFrmhead->sa.saLen.sa.saType) {
    case DLT69845_SA_TYPE_SINGLE:
		fprintf(stderr, "single address\n");
		break;
    case DLT69845_SA_TYPE_WILDCARD:
		fprintf(stderr, "wildcard address\n");
		break;
    case DLT69845_SA_TYPE_GROUP:
		fprintf(stderr, "group address\n");
		break;
    case DLT69845_SA_TYPE_BROADCAST:
		fprintf(stderr, "broadcast address\n");
		break;
	default:
		break;
	}

    /*copy server address*/
    if(NULL != pFrmhead->sa.sa)
        return FALSE;

    pFrmhead->sa.sa = calloc(1, pFrmhead->sa.saLen.sa.saLen*sizeof(*(pFrmhead->sa.sa)));
    if(NULL == pFrmhead->sa.sa)
        return FALSE;

    memcpy((u8*)(pFrmhead->sa.sa), (p+DLT69845_HEAD_LEN_BEFORE_SA), pFrmhead->sa.saLen.sa.saLen);
    DEBUG_TIME_LINE("server id:");
    printBuf((u8*)pFrmhead->sa.sa, pFrmhead->sa.saLen.sa.saLen, 0, 1);
    /*copy server address done*/

    //check frame length
    if(*bufSize < (pFrmhead->frmLen.len.len+2))
        return FALSE;

    *bufSize = pFrmhead->frmLen.len.len + 2;

    pFrmhead->ca = *(p+DLT69845_HEAD_LEN_BEFORE_SA+pFrmhead->sa.saLen.sa.saLen);
    DEBUG_TIME_LINE("client id: %02X", pFrmhead->ca);

    /*check head's crc*/
    memcpy(&pFrmhead->headChk,
           (p+DLT69845_HEAD_LEN_BEFORE_SA+pFrmhead->sa.saLen.sa.saLen+sizeof(u8)),
           sizeof(u16));

    crc = fcs16((p+1), (pFrmhead->headLen-3));

    if(crc != pFrmhead->headChk)
        return FALSE;
    /*check head's crc done*/

    /*check frame's crc*/
    memcpy(&crc, p + *bufSize-3, sizeof(u16));

    fcs = fcs16((p+1), (pFrmhead->frmLen.len.len-2));
    if(crc != fcs)
        return FALSE;
    /*check frame's crc done*/

    //remove Scrambling Code if existing
    if (pFrmhead->ctlChar.ctl.sc == DLT69845_SCAMBLE_WITH)
        scamCode(buf+pFrmhead->headLen, pFrmhead->apduLen, FALSE);

    return TRUE;
}

/*
 * decodeFrame 698 frame
 * @buf: frame buffer
 * @bufSize: frame length
 * @return: FALSE-frame invalid; TRUE-frame valid.
 */
boolean decodeFrame(u8* buf, u16 bufSize, frm698_p pFrame)
{
    if((NULL == buf) || (0 == bufSize) || (NULL == pFrame))
        return FALSE;

    boolean ret = TRUE;
    u8* pApduBuf = NULL;
    u16 apduLen = 0;

    ret = checkFrame(buf, &bufSize, &pFrame->head);
    if( FALSE == ret) {
        DEBUG_TIME_LINE("frame invalid");
        ret = FALSE;
        goto onret;
    }

    pApduBuf = buf + pFrame->head.headLen;
    apduLen = bufSize-pFrame->head.headLen - 3;
    switch (pFrame->head.ctlChar.ctl.func) {
    case DLT69845_FUNC_LINK_MANAGE:
        ret = decodeLinkApdu(pApduBuf, apduLen, &pFrame->apdu);
        break;
    case DLT69845_FUNC_LINK_UERDATA:
        ret = userDataManager(buf, bufSize, &pFrame->apdu);
        break;
    default:
        break;
    }

 onret:
    if(pFrame->head.sa.sa != NULL)
        free(pFrame->head.sa.sa);

    return ret;
}

/*
 * decodeFrame 698 frame
 * @buf: frame buffer
 * @bufSize: frame length
 * @return: FALSE-frame invalid; TRUE-frame valid.
 */
boolean encodeFrame(u8* buf, u16* bufSize, frm698_p pFrame)
{
    if((NULL == buf) || (0 == bufSize) || (NULL == pFrame))
        return FALSE;

    boolean ret = TRUE;
    u8* pApduBuf = NULL;
    u16 apduLen = 0;

    ret = checkFrame(buf, &bufSize, &pFrame->head);
    if( FALSE == ret) {
        DEBUG_TIME_LINE("frame invalid");
        ret = FALSE;
        goto onret;
    }

    pApduBuf = buf + pFrame->head.headLen;
    apduLen = bufSize-pFrame->head.headLen - 3;
    switch (pFrame->head.ctlChar.ctl.func) {
    case DLT69845_FUNC_LINK_MANAGE:
        ret = decodeLinkApdu(pApduBuf, apduLen, &pFrame->apdu);
        break;
    case DLT69845_FUNC_LINK_UERDATA:
        ret = userDataManager(buf, bufSize, &pFrame->apdu);
        break;
    default:
        break;
    }

 onret:
    if(pFrame->head.sa.sa != NULL)
        free(pFrame->head.sa.sa);

    return ret;
}
