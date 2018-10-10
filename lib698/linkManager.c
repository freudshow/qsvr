#include "linkManager.h"

/*
 * linkManager 只负责解析
 * 和动作应用层的apdu,
 * 链路层的信息直接拿来用.
 * @buf: 指向apdu的缓冲区
 * @bufSize: apdu缓冲区的长度
 * @return: 执行正常, 返回TRUE
 *          出错, 返回FALSE
 */
boolean linkManager(u8* buf, u16 bufSize, frmHead_s* pFrmHead)
{
    u8 ret = TRUE;
    u8* pbuf = buf;



    return ret;
}
