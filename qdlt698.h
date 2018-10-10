#ifndef QDLT698_H
#define QDLT698_H

#include <QObject>

class QDlt698
{
public:
    explicit QDlt698();
    ~QDlt698();

    QString     startChar;//start code
    qint32      frmLen;//frame length,
    QString     func;//function code
    QString     Scrambling;//Scrambling Code flag
    QString     divS;//divide frame flag.
    QString     prm;//promote flag
    QString     dir;//direction flag
    QString     sa;//server address
    QString     ca;//client address
    QString     headChk;//crc of head
    qint8       headLen;//length of frame's head, including startChar
};

#endif // QDLT698_H
