#include "mytcpsockets.h"
#include <QDataStream>
#include <QCryptographicHash>
#include <QSettings>
#include <QMessageBox>
#include <QDebug>

MySocket::MySocket(QObject *parent, qintptr socket) : QTcpSocket(parent)
{
    Q_UNUSED(parent);

    this->setSocketDescriptor(socket);

    connect(this, SIGNAL(readyRead()), this, SLOT(on_readData()));

    bytesReceived = 0;
    gotBytes = 0;
    fileBytes = 0;
    fileNameSize = 0;

    connect(this, &MySocket::disconnected, []{
        qDebug() << "disconnected";
    });

    qDebug() << "Client IP:" << this->peerAddress() << this->peerPort() << this->peerName()
             << "connect!";
}

MySocket::~MySocket()
{
    qDebug() << "MySocket bye!";
}

void MySocket::on_readData()
{
    QByteArray b = readAll();
    qDebug() << b;
}

void MySocket::on_caller_closed()
{
    qDebug() << "on_caller_closed()";
    disconnectFromHost();
}
