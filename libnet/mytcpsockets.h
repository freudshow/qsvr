#ifndef MYSOCKET_H
#define MYSOCKET_H

#include <QTcpSocket>
#include <QHostAddress>
#include <QFile>


class MySocket : public QTcpSocket
{
    Q_OBJECT
public:
    MySocket(QObject *parent, qintptr socket);
    ~MySocket();

public slots:
    void on_readData();
    void on_caller_closed();

public:
    QFile *file;
    QByteArray inBlock, outBlcok;
    int photoCount = 0;

    qint64 gotBytes = 0, fileBytes, fileNameSize, bytesReceived;
    qint64 fileSize, sendTotalBytes, bytesToWrite, bytesWritten;
    QString fileName;
    QString receivedMd5;

    int socketFlag;

    int photoN;
    int currentLocation;
    QStringList fileNameList;
    bool updateVersion;
    QString Tips;

    bool firstConnection = false;

    void replyMd5(bool boole = true);

    void sendFile(bool boole = true);

signals:
    void sendReceivedFileName(const QString &fileName);
};

#endif // MYSOCKET_H
