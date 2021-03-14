#include "mytcpserver.h"

#include "mytcpserver.h"
#include "mythread.h"
#include <QDebug>

myTcpServer::myTcpServer(QObject *parent) : QTcpServer(parent)
{
    Q_UNUSED(parent);
}

myTcpServer::~myTcpServer()
{
    qDebug() << "myTcpServer bye!";
}

void myTcpServer::incomingConnection(qintptr socketDescriptor)
{
    MyThread *thread = new MyThread(this, socketDescriptor);
    connect(this, SIGNAL(serverClosed()), thread, SIGNAL(threadClosed()));
    thread->start();
}


