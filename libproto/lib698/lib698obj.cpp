#include "lib698obj.h"

lib698Obj::lib698Obj(QObject *parent) : QObject(parent)
{

}


void lib698Obj::pushBytes(QByteArray b)
{
    m_buf.append(b);
}

QByteArray lib698Obj::getOneFrameFromBuf()
{

}

void lib698Obj::getBlockSlice(QByteArray b)
{
    Q_UNUSED(b);
}

