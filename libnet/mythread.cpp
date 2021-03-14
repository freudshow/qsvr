#include "mythread.h"
#include "mytcpsockets.h"
#include <QDebug>

MyThread::MyThread(QObject *parent, qintptr socket) : QThread(parent)
{
    this->m_intPtr = socket;
    m_socket = Q_NULLPTR;
    qDebug() << m_intPtr;

    connect(this, &MyThread::finished, [&] {
        qDebug() << "thread is over";
        emit threadClosed();
    });
    connect(this, &MyThread::finished, &MyThread::deleteLater);
}

MyThread::~MyThread()
{
    qDebug() << "MyThread bye!";
}

void MyThread::run()
{
    m_socket = new MySocket(0, this->m_intPtr);
    connect(m_socket, &MySocket::disconnected, this, &MyThread::quit, Qt::DirectConnection);
    connect(m_socket, &MySocket::sendReceivedFileName, this, &MyThread::sendReceivedFileName);
    connect(this, SIGNAL(threadClosed()), m_socket, SLOT(on_caller_closed()));
    this->exec();
}
