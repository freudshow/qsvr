#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QSettings>

#include "libutil/basedef.h"
#include "libcom/libcom.h"
#include "libnet/libnet.h"
#include "ui/comconfigform.h"
#include "ui/netconfigform.h"
#include "ui/setrs485.h"
#include "ui/setlogicaddr.h"

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
    void on_btnOpenCom_clicked();
    void on_btnListenTcp_clicked();
	void on_pushButton_Send_clicked();
    void on_Button_ClrRcv_clicked();

    void on_actionComconfig_triggered();
    void on_actionNetconfig_triggered();
    void on_actionSetLogicAddr_triggered();
    void on_actionSetRS485Config_triggered();

    void serverNewConnect();
    void socketReadData();
    void socketDisconnected();

    void comReadData(QByteArray);

    void sendMsg();
    void setRecvCursor();
    QString byteArrayToString(QByteArray, bool);

private:
	Ui::MainWindow *ui;
    comConfigForm *comUi;
    netConfigForm *netUi;
    setLogicAddr *logicAddrUi;
    setRS485 *rs485Ui;

	QTcpServer* pServer;
	QTcpSocket* pSocket;
    QString m_tcpPort;
    QSettings *m_config;
    bool listenState;

    QList<QTcpSocket*> tcpClientList;
    QTcpSocket *pCurrentClient;

    comObj *m_comObj;
    QThread *m_comThread;

signals:
    void sendBuf(QByteArray);
};

#endif // MAINWINDOW_H
