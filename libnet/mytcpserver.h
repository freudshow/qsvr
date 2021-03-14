#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H


#include <QTcpServer>
#include <QList>


class myTread;

class myTcpServer : public QTcpServer
{
    Q_OBJECT

public:
    myTcpServer(QObject *parent = 0);
    ~myTcpServer();

    virtual void incomingConnection(qintptr socketDescriptor);
signals:
    void serverClosed();
};

#endif // MYTCPSERVER_H
