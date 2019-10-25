#ifndef LIB698OBJ_H
#define LIB698OBJ_H

#include "lib698.h"
#include <QObject>
#include <QByteArray>

class lib698Obj : public QObject
{
    Q_OBJECT
public:
    explicit lib698Obj(QObject *parent = 0);

private:
    QByteArray m_buf;

signals:
    void sendOneFrame(QByteArray b);
    void requestBlockSlice(int i); //请求第i块文件数据

public slots:
    void pushBytes(QByteArray b);     //将系统读到的数据入队列
    QByteArray getOneFrameFromBuf();  //从缓存中拿到一帧报文
    void getBlockSlice(QByteArray b); //接收文件数据

};

#endif // LIB698OBJ_H
