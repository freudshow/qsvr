#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QSettings>

#include "libutil/basedef.h"
#include "libcom/libcom.h"
#include "libnet/libnet.h"
#include "ui/comconfigform.h"
#include "ui/netconfigform.h"
#include "ui/setrs485.h"
#include "ui/setlogicaddr.h"
#include "ui/toolsdialog.h"
#include "ui/filessum.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = Q_NULLPTR);
    ~MainWindow();

public slots:


private slots:
    void on_btnOpenCom_clicked();
    void on_btnListenTcp_clicked();
    void on_pushButton_Send_clicked();
    void on_Button_ClrRcv_clicked();

    void on_actionComconfig_triggered();
    void on_actionNetconfig_triggered();
    void on_actionSetLogicAddr_triggered();
    void on_actionSetRS485Config_triggered();
    void on_actionSums_triggered();
    void on_actionFilesum_triggered();
    void delToolDiag();
    void delFileSum();

    void serverNewConnect();
    void newclient(clientInfo_s);
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
    toolsDialog* toolsDiag;
    filesSum *filesumWgt;

    QTcpServer* pServer;
    QTcpSocket* pSocket;
    QUdpSocket* m_pUdpSoket;
    QString m_tcpPort;
    QSettings *m_config;
    netSvr *m_netSvr;
    QList<clientInfo_s> m_clientList;

    QList<QTcpSocket*> tcpClientList;
    QTcpSocket *pCurrentClient;

    comObj *m_comObj;
    QThread *m_comThread;

signals:
    void sendBuf(QByteArray);
};

#endif // MAINWINDOW_H
