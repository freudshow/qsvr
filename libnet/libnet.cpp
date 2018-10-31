#include "libnet.h"

threadObj::threadObj(qintptr ID, QObject *parent) :
    QThread(parent)
{
    this->socketDescriptor = ID;
}

void threadObj::run()
{
    qDebug() << " Thread started: " << QThread::currentThreadId();

    m_socket = new QTcpSocket();
    connect(this, SIGNAL(error()), this, SLOT(deleteLater()));
    if(!m_socket->setSocketDescriptor(this->socketDescriptor)) {
        emit error(m_socket->error());
        return;
    }

    connect(m_socket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::DirectConnection);
    connect(m_socket, SIGNAL(disconnected()), this, SLOT(disconnected()));

    qDebug() << "socketDescriptor: " << socketDescriptor << " Client connected";
    qDebug() << "Client:" << m_socket->peerAddress()
             << m_socket->peerPort() << m_socket->peerName()
             << "connect!";

    exec();
}

void threadObj::readyRead()
{
    QByteArray b = m_socket->readAll();
    qDebug() << socketDescriptor << " Data in: " << b;
    emit dataIn(b);
}

void threadObj::sendFrame(QByteArray b)
{
    m_socket->write(b);
}

void threadObj::disconnected()
{
    qDebug() << socketDescriptor << " Disconnected";

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
    thread->start();
}

void netSvr::socketError(QTcpSocket::SocketError err)
{
    qDebug() << "socketError";
}
