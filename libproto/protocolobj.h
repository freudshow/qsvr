#ifndef PROTOCOLOBJ_H
#define PROTOCOLOBJ_H

#include <QObject>
#include <QByteArray>
#include "libutil/basedef.h"

typedef enum protoTypes {
    e_698_type = 0,
    e_188_type,
    e_645_type,
    e_3761_type,
    e_3762_type,
    e_unknow_type
} protoTypes_e;

class protocolObj : public QObject
{
public:
    protocolObj();
    ~protocolObj();

public slots:
    protoTypes_e analyFrame(QByteArray buf);
    void processFrame(QByteArray);

signals:
    void sendFrame(QByteArray);

};

#endif // PROTOCOLOBJ_H
