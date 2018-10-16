#include <QDebug>
#include "libcom.h"

comObj::comObj(QObject* parent) : QObject(parent)
{
    m_serialPort = Q_NULLPTR;
    m_timer = Q_NULLPTR;
}

comObj::~comObj()
{
    RELEASE_POINTER_RESOURCE(m_serialPort)
    RELEASE_POINTER_RESOURCE(m_timer)
}

bool comObj::openCom(comInfoPtr pComInfo)
{
    qDebug() << "comObject::openCom threadId: " << QThread::currentThreadId();

    if(Q_NULLPTR == pComInfo)
        return false;

    if (Q_NULLPTR == m_serialPort) {
#ifdef Q_OS_LINUX
        m_serialPort = new QSerialPort("/dev/" + pComInfo->portName);
#elif defined (Q_OS_WIN)
        m_serialPort = new QSerialPort();
        m_serialPort->setPortName(pComInfo->portName);
#endif
    }

    m_serialPort->setBaudRate(pComInfo->baudrate);
    m_serialPort->setDataBits(pComInfo->databits);
    m_serialPort->setParity(pComInfo->parity);
    m_serialPort->setStopBits(pComInfo->stopbits);
    m_serialPort->setFlowControl(QSerialPort::NoFlowControl);
    connect(m_serialPort, SIGNAL(readyRead()), this, SLOT(readBuf()));
    return m_serialPort->open(QSerialPort::ReadWrite);
}

void comObj::startThread()
{
    qDebug() << "### comObject::startThread thread:" << QThread::currentThreadId();
    m_readBuf.clear();
    if (Q_NULLPTR == m_timer)
        m_timer = new QTimer;

    m_timer->setInterval(TIME_OUT);
    m_timer->stop();
    connect(m_timer, SIGNAL(timeout()), this, SLOT(sendData()));
}

void comObj::closeCom()
{
    m_serialPort->close();
}

/*
 * 在TIME_OUT ms之内, 不断的读取
 * 串口数据, 并将其追加到接收缓冲区
 */
void comObj::readBuf()
{
    QByteArray buf = m_serialPort->readAll();

    if (!buf.isEmpty())
        m_readBuf.append(buf);
}

/*
 * 串口协议一般是主-从模式,
 * 从设备只有等到主设备的命
 * 令后才发送报文, 所以每次
 * 发送下行报文之前都要先清
 * 空接收缓冲区.
 */
void comObj::sendBuf(QByteArray buf)
{
    m_readBuf.clear();
    m_serialPort->write(buf);
    m_timer->start(TIME_OUT);//等待串口数据全部接收完成
}

/*
 * 当发送命令后, 等待的时间
 * 超过预定义的时间(TIME_OUT ms)
 * 就把接收缓冲区发送给接收者.
 */
void comObj::sendData()
{
    qDebug() << "comObject::sendData" << QThread::currentThreadId();
    m_timer->stop();
    emit readBufReady(m_readBuf);
}
