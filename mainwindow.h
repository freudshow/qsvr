#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include "libutil/basedef.h"
#include "ui/comconfigform.h"
#include "ui/netconfigform.h"

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = Q_NULLPTR);
	~MainWindow();

private slots:
	void on_pushButton_Listen_clicked();
	void on_pushButton_Send_clicked();
    void on_Button_ClrRcv_clicked();
    void on_actionComconfig_triggered();
    void on_actionNetconfig_triggered();


    void serverNewConnect();
    void socketReadData();
    void socketDisconnected();
    void sendMsg();
    void setRecvCursor();
    QString byteArrayToString(QByteArray, bool);

private:
	Ui::MainWindow *ui;
    comConfigForm *comUi;
    netConfigForm *netUi;

	QTcpServer* pServer;
	QTcpSocket* pSocket;
    bool listenState;

    QList<QTcpSocket*> tcpClientList;
    QTcpSocket *pCurrentClient;
};

#endif // MAINWINDOW_H
