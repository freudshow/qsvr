#ifndef LIBNET_H
#define LIBNET_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include "libutil/basedef.h"

class libnet : public QObject
{
    Q_OBJECT
public:
    explicit libnet(QObject *parent = nullptr);

signals:

public slots:
};

#endif // LIBNET_H
