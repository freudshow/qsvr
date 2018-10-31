#ifndef LIBNET_H
#define LIBNET_H

/*
 * reference: https://www.bogotobogo.com/Qt/Qt5_QTcpServer_Multithreaded_Client_Server.php
 *            https://www.bogotobogo.com/Qt/Qt5_QTcpServer_Client_Server.php
 */
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>
#include "libutil/basedef.h"


class threadObj : public QThread
{
    Q_OBJECT

public:
    explicit threadObj(qintptr ID, QObject *parent = Q_NULLPTR);

    void run();

signals:
    void dataIn(QByteArray);
    void error(QTcpSocket::SocketError socketerror);

public slots:
    void readyRead();
    void writeFrame(QByteArray);
    void disconnected();

private:
    QTcpSocket *m_socket;
    qintptr m_socketDescriptor;

    QByteArray m_buffer;
};

class netSvr : public QTcpServer
{
    Q_OBJECT
public:
    explicit netSvr(QObject *parent = Q_NULLPTR);
    bool startServer(quint16);
signals:

public slots:
    void socketError(QTcpSocket::SocketError);

protected:
    void incomingConnection(qintptr socketDescriptor);

};

#endif // LIBNET_H
