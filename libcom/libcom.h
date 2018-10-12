#ifndef LIBCOM_H
#define LIBCOM_H

#include <QObject>
#include <QDebug>
#include <QThread>
#include <QTimer>
#include <QSerialPort>
#include <QSerialPortInfo>
#include "libutil/basedef.h"


typedef struct {
    QString					portName;
    QSerialPort::BaudRate	baudrate;
    QSerialPort::DataBits	databits;
    QSerialPort::Parity		parity;
    QSerialPort::StopBits	stopbits;
} comInfoStr;
typedef comInfoStr* comInfoPtr;

class comObj : public QObject
{
    Q_OBJECT
public:
    comObj(QObject* parent = Q_NULLPTR);
    ~comObj();
private:
    QSerialPort* m_serialPort;
    QByteArray m_readBuf;//数据缓冲区
    QTimer* m_timer;

signals:
    void openComOK();
    void openComFail();
    void finished();
    void readBufReady(QByteArray);
public slots :
    void startThread();

    void openCom(comInfoPtr);
    void closeCom();

    void sendBuf(QByteArray);

private slots :
    void readBuf();
    void sendData();
};

#endif // LIBCOM_H
