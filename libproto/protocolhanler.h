#ifndef PROTOCOLHANLER_H
#define PROTOCOLHANLER_H

#include "protocolobj.h"
#include <QObject>

class ProtocolHanler : public QObject
{
    Q_OBJECT
public:
    explicit ProtocolHanler(QObject *parent = nullptr);

signals:

};

#endif // PROTOCOLHANLER_H
