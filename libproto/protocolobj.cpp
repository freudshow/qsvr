#include "protocolobj.h"

protocolObj::protocolObj()
{

}

protocolObj::~protocolObj()
{

}

protoTypes_e protocolObj::analyFrame(QByteArray buf)
{
    protoTypes_e t = e_unknow_type;
    Q_UNUSED(buf)

    return t;
}
