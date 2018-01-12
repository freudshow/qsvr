#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include "basedef.h"

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

private slots:
	void on_pushButton_Listen_clicked();
	void on_pushButton_Send_clicked();
    void serverNewConnect();
    void socketReadData();
    void socketDisconnected();
    void sendMsg();
    QString byteArrayToString(QByteArray, bool);
    bool byteArray2CharBuf(QByteArray, char*, u32);

private:
	Ui::MainWindow *ui;
	QTcpServer* pServer;
	QTcpSocket* pSocket;
    bool listenState;
};

#endif // MAINWINDOW_H
