#ifndef QDLT698_H
#define QDLT698_H

#include <QObject>

class QDlt698
{
public:
    QDlt698();

    QString     startChar;//start code
    qint32      frmLen;//frame length,
    ctl_u       ctlChar;//ctrol code
    QString     sa;//server address
    QString     ca;//client address
    QString     headChk;//crc of head
    qint8       headLen;//length of frame's head, including startChar
};

#endif // QDLT698_H
