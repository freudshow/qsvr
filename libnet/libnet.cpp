#include "libnet.h"

threadObj::threadObj(qintptr ID, QObject *parent) :
    QThread(parent)
{
    this->m_socketDescriptor = ID;
    m_clientInfo.connected = false;
    m_clientInfo.hostAddr.clear();
    m_clientInfo.m_logicAddr.clear();
    m_clientInfo.m_protoType = protoTypes_e::e_unknow_type;
}

void threadObj::run()
{
    qDebug() << " Thread started: " << QThread::currentThreadId();

    m_socket = new QTcpSocket();
    connect(this, SIGNAL(error()), this, SLOT(deleteLater()));
    if(!m_socket->setSocketDescriptor(this->m_socketDescriptor)) {
        emit error(m_socket->error());
        return;
    }


    connect(m_socket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::DirectConnection);
    connect(m_socket, SIGNAL(disconnected()), this, SLOT(disconnected()));

    qDebug() << "socketDescriptor: " << m_socketDescriptor << " Client connected";
    qDebug() << "Client:" << m_socket->peerAddress()
             << m_socket->peerPort() << m_socket->peerName()
             << "connect!";
    m_clientInfo.connected = true;
    m_clientInfo.hostAddr = tr("%1:%2-%3").arg(m_socket->peerAddress().toString().split("::ffff:")[1])\
                                       .arg(m_socket->peerPort()).arg(m_socketDescriptor);
    emit clientInfo(m_clientInfo);
    exec();
}

void threadObj::readyRead()
{
    QByteArray b = m_socket->readAll();
    m_buffer.append(b);
    qDebug() << m_socketDescriptor << " Data in: " << b;
    emit dataIn(b);
}

void threadObj::writeFrame(QByteArray b)
{
    m_socket->write(b);
}

void threadObj::disconnected()
{
    qDebug() << m_socketDescriptor << " Disconnected";

    m_socket->deleteLater();
    exit(0);
}

netSvr::netSvr(QObject *parent) :
    QTcpServer(parent)
{
}

bool netSvr::startServer(quint16 port)
{
    if(!this->listen(QHostAddress::Any, port)) {
        qDebug() << "Could not start server" << this->errorString();
        return false;
    }

    qDebug() << "Listening to port " << port << "...";
    return false;
}

void netSvr::incomingConnection(qintptr socketDescriptor)
{
    qDebug() << socketDescriptor << " Connecting...";

    threadObj *thread = new threadObj(socketDescriptor, this);
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    connect(thread, SIGNAL(error(QTcpSocket::SocketError)), this, SLOT(socketError(QTcpSocket::SocketError)));
    connect(thread, SIGNAL(dataIn(QByteArray)), this, SIGNAL(dataIn(QByteArray)));
    connect(thread, SIGNAL(clientInfo(clientInfo_s)), this, SIGNAL(clientInfo(clientInfo_s)));
    thread->start();
}

void netSvr::socketError(QTcpSocket::SocketError err)
{
    qDebug() << "socketError";
}
