#ifndef MYTREAD_H
#define MYTREAD_H

#include "mytcpsockets.h"
#include <QThread>
#include <QWidget>

class MyThread : public QThread
{
    Q_OBJECT
public:
    MyThread(QObject *parent, qintptr socket);
    ~MyThread();

private:
    qintptr m_intPtr;
    MySocket *m_socket;
    virtual void run();

signals:
    void sendReceivedFileName(const QString &fileName);
    void threadClosed();
};


#endif // MYTREAD_H
