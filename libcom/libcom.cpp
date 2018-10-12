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

void comObj::openCom(comInfoPtr pComInfo)
{
    qDebug() << "comObject::openCom threadId: " << QThread::currentThreadId();
    //comInfoPtr pComInfo = &comInfo;
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
    if (m_serialPort->open(QSerialPort::ReadWrite))
        emit openComOK();
    else
        emit openComFail();
    connect(m_serialPort, SIGNAL(readyRead()), this, SLOT(readBuf()));
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

void comObj::readBuf()
{
    QByteArray buf = m_serialPort->readAll();

    if (!buf.isEmpty())
        m_readBuf.append(buf);
}

void comObj::sendBuf(QByteArray buf)
{
    qDebug() << "sendBuf get signal";
    m_readBuf.clear();

    m_serialPort->write(buf);

    m_timer->start(TIME_OUT);//等待串口数据全部接收完成
}

void comObj::sendData()
{
    qDebug() << "comObject::sendData" << QThread::currentThreadId();
    m_timer->stop();
    emit readBufReady(m_readBuf);
}
