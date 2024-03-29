#include <QMessageBox>
#include <QDateTime>
#include <QTextCursor>
#include <QSerialPort>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "libproto/lib698/lib698.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->pushButton_Send->setEnabled(false);
    ui->checkBox_RecvHex->setChecked(true);
    ui->checkBox_SendHex->setChecked(true);

    connect(ui->textEdit_Recv, &QTextEdit::textChanged, this, &MainWindow::setRecvCursor);

    pServer = new QTcpServer();
    pSocket = Q_NULLPTR;

    //连接信号槽
    connect(pServer,&QTcpServer::newConnection,this,&MainWindow::serverNewConnect);

    m_netSvr = new netSvr(this);
    connect(m_netSvr, SIGNAL(clientInfo(clientInfo_s)), this, SLOT(newclient(clientInfo_s)));
    m_clientList.clear();

    comUi       = Q_NULLPTR;
    netUi       = Q_NULLPTR;
    logicAddrUi = Q_NULLPTR;
    rs485Ui     = Q_NULLPTR;
    toolsDiag   = Q_NULLPTR;
    filesumWgt  = Q_NULLPTR;
    m_config    = Q_NULLPTR;
    m_comObj    = Q_NULLPTR;
    m_comThread = Q_NULLPTR;
}

MainWindow::~MainWindow()
{
    pServer->close();
    pServer->deleteLater();
    m_netSvr->close();
    m_netSvr->deleteLater();
    RELEASE_POINTER_RESOURCE(m_config)
    RELEASE_POINTER_RESOURCE(comUi)
    RELEASE_POINTER_RESOURCE(netUi)
    RELEASE_POINTER_RESOURCE(logicAddrUi)
    RELEASE_POINTER_RESOURCE(rs485Ui)
    RELEASE_POINTER_RESOURCE(ui);
}

void MainWindow::setRecvCursor()
{
    QTextCursor cursor=ui->textEdit_Recv->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->textEdit_Recv->setTextCursor(cursor);
}

void MainWindow::on_btnOpenCom_clicked()
{
    comInfoStr comInfo;

    if(Q_NULLPTR == m_config)
        m_config = new QSettings("config/config.ini", QSettings::IniFormat);
    if(Q_NULLPTR == m_comObj)
        m_comObj = new comObj();
    if(Q_NULLPTR == m_comThread)
        m_comThread = new QThread();

    comInfo.portName = m_config->value("ComSettings/ComName").toString();
    comInfo.baudrate = static_cast<QSerialPort::BaudRate>(m_config->value("ComSettings/BaudRate").toInt());
    comInfo.databits = static_cast<QSerialPort::DataBits>(m_config->value("ComSettings/DataBit").toInt());

    switch(m_config->value("ComSettings/Parity").toInt()){
        case PARITY_NO:
            comInfo.parity = QSerialPort::NoParity;
            break;
        case PARITY_ODD:
            comInfo.parity = QSerialPort::OddParity;
            break;
        case PARITY_EVEN:
           comInfo.parity = QSerialPort::EvenParity;
            break;
        default:
            comInfo.parity = QSerialPort::NoParity;
            qDebug("set to default : NoParity");
            break;
    }

    switch(m_config->value("ComSettings/StopBit").toInt()){
        case STOP_ONE:
            comInfo.stopbits = QSerialPort::OneStop;
            break;
        case STOP_ONEANDHALF:
            comInfo.stopbits = QSerialPort::OneAndHalfStop;
            break;
        case STOP_TWO:
           comInfo.stopbits = QSerialPort::TwoStop;
            break;
        default:
            comInfo.stopbits = QSerialPort::OneStop;
            qDebug("set to default : OneStop");
            break;
    }

    if(m_comObj->openCom(&comInfo)) {
        connect(m_comObj, SIGNAL(readBufReady(QByteArray)), this, SLOT(comReadData(QByteArray)));
        connect(this, SIGNAL(sendBuf(QByteArray)), m_comObj, SLOT(sendBuf(QByteArray)));

        ui->btnOpenCom->setEnabled(false);
        ui->pushButton_Send->setEnabled(true);
        QMessageBox::information(this, tr("Hint"), tr("open com OK!"), QMessageBox::Ok);
    } else {
        QMessageBox::critical(this, tr("critical"), tr("open com fail!"), QMessageBox::Ok);
        return;
    }

    CONNECT_THREAD(m_comObj, startThread, m_comThread);
}

void MainWindow::comReadData(QByteArray buffer)
{
    bool isHex = ui->checkBox_RecvHex->isChecked();
    frm698_s frm698 = {};
    memset(&frm698, 0, sizeof(frm698));

    if(!buffer.isEmpty()) {
        u8 ret = decodeFrame((u8*)(buffer.data()), buffer.length(), &frm698);
        if(TRUE == ret) {
            qDebug() << "frame valid";
        } else {
            qDebug() << "frame invalid";
        }


        switch (frm698.head.ctlChar.dirPrm.dpAssem) {
        case DLT69845_DIR_PRM_CLT_RESPONSE:

            break;
        case DLT69845_DIR_PRM_CLT_REQUEST:

            break;
        case DLT69845_DIR_PRM_SRV_REPORT:

            break;
        case DLT69845_DIR_PRM_SRV_RESPONSE:

            break;
        default:
            break;
        }


        ui->textEdit_Recv->append(isHex ? byteArrayToString(buffer, true) : tr(buffer));
    }
}

void MainWindow::on_btnListenTcp_clicked()
{

    qDebug() << "listenState: " << pServer->isListening();


    if(!pServer->isListening()) {
        if(Q_NULLPTR == m_config)
            m_config =  new QSettings("config/config.ini", QSettings::IniFormat);

        m_tcpPort = m_config->value("tcp/port").toString();
        if(m_tcpPort.length() == 0) {
            QMessageBox::warning(this, tr("Hint"), tr("please set tcp port first!"), QMessageBox::Ok);
            return;
        }

        quint16 port = static_cast<quint16>(m_tcpPort.toUInt());
        if(!pServer->listen(QHostAddress::Any, port)){
            qDebug() << pServer->errorString();
            QMessageBox::critical(this, tr("Hint"), pServer->errorString(), QMessageBox::Ok);
            return;
        }

        qDebug()<< "Listen succeessfully!";
    } else {
        if(Q_NULLPTR != pSocket) {
            if (pSocket->state() == QAbstractSocket::ConnectedState)
                pSocket->disconnectFromHost();
            pSocket = Q_NULLPTR;
        }
        pServer->close();
    }

    ui->btnListenTcp->setText(pServer->isListening() ? tr("Don't Listen") : tr("Listen"));
    ui->pushButton_Send->setEnabled(pServer->isListening());
}

void MainWindow::on_pushButton_Send_clicked()
{
    sendMsg();
    qDebug() << "Send: " << ui->textEdit_Send->toPlainText();
}

void MainWindow::on_Button_ClrRcv_clicked()
{
    ui->textEdit_Recv->clear();
}

void MainWindow::on_actionComconfig_triggered()
{
    if(Q_NULLPTR != comUi)
        delete comUi;

    comUi = new comConfigForm();
    comUi->setWindowModality(Qt::ApplicationModal);
    comUi->show();
}

void MainWindow::on_actionNetconfig_triggered()
{
    if(Q_NULLPTR != netUi)
        delete netUi;

    netUi = new netConfigForm();
    netUi->setWindowModality(Qt::ApplicationModal);
    netUi->show();
}

void MainWindow::on_actionSetLogicAddr_triggered()
{
//    if(Q_NULLPTR != logicAddrUi)
//        delete logicAddrUi;

//    logicAddrUi = new setLogicAddr();
//    logicAddrUi->setWindowModality(Qt::ApplicationModal);
//    logicAddrUi->show();
        QByteArray sendbuf("\x68\x1b\x00\x43\x03\x07\x00\x00\x00\x10\xf6\xa4\x06\x01\x00\x40\x01\x02\x00\x09\x04\x00\x00\x00\x08\x00\xe4\x41\x16", 29);
        emit sendBuf(sendbuf);
}

void MainWindow::on_actionSetRS485Config_triggered()
{
    if(Q_NULLPTR != rs485Ui)
        delete rs485Ui;

    rs485Ui = new setRS485();
    rs485Ui->setWindowModality(Qt::ApplicationModal);
    rs485Ui->show();

    QByteArray sendbuf("0x68\x24\x00\x43\x03\x07\x00\x00\x00\x10\x73\x3a\x07\x01\x00\xf2\x01\x7f\x00\x02\x03\x51\xf2\x01\x02\x01\x5f\x03\x02\x08\x01\x00\x16\x01\x00\x28\xb2\x16");

    emit sendBuf(sendbuf);
}

void MainWindow::on_actionSums_triggered()
{
    toolsDiag = new toolsDialog();
    toolsDiag->setAttribute(Qt::WA_DeleteOnClose, true);
    toolsDiag->setWindowModality(Qt::ApplicationModal);
    QObject::connect(toolsDiag, SIGNAL(toolExit()), this, SLOT(delToolDiag()));
    toolsDiag->show();
}

void MainWindow::delToolDiag()
{
    toolsDiag = Q_NULLPTR;
}

void MainWindow::on_actionFilesum_triggered()
{
    filesumWgt = new filesSum();
    filesumWgt->setAttribute(Qt::WA_DeleteOnClose, true);
    filesumWgt->setWindowModality(Qt::ApplicationModal);
    QObject::connect(filesumWgt, SIGNAL(fileClose()), this, SLOT(delFileSum()));
    filesumWgt->show();
}

void MainWindow::delFileSum()
{
    filesumWgt = Q_NULLPTR;
}

void MainWindow::sendMsg()
{
    QByteArray buf;
    if (ui->checkBox_SendHex->isChecked()) {
        int i = 0;
        QString str;
        bool ok;
        char data;
        QStringList list;

        str = ui->textEdit_Send->toPlainText();
        list = str.split(" ");
        for (i = 0; i < list.count(); i++) {
            if (list.at(i) == " ")
                continue;
            if (list.at(i).isEmpty())
                continue;
            data = static_cast<char>(list.at(i).toInt(&ok, 16));
            if (!ok){
                QMessageBox::information(this, tr("Hint"), tr("data format error"), QMessageBox::Ok);
                return;
            }
            buf.append(data);
        }
    } else {
#if QT_VERSION < 0x050000
        buf = ui->textEdit_Send->toPlainText().toAscii();
#else
        buf = ui->textEdit_Send->toPlainText().toLocal8Bit();
#endif
    }

    if(Q_NULLPTR != pSocket) {
        pSocket->write(buf);
        pSocket->flush();
    }

    emit sendBuf(buf);

    QString str = ui->textEdit_Recv->toPlainText();
    str += byteArrayToString(buf, false);
    ui->textEdit_Recv->setText(str);
    qDebug() << tr("send data success");
}

void MainWindow::serverNewConnect()
{
    pSocket = pServer->nextPendingConnection();

    QObject::connect(pSocket, &QTcpSocket::readyRead, this, &MainWindow::socketReadData);
    QObject::connect(pSocket, &QTcpSocket::disconnected, this, &MainWindow::socketDisconnected);
    ui->pushButton_Send->setEnabled(true);

    qDebug() << "Client IP:" << pSocket->peerAddress()
             << pSocket->peerPort() << pSocket->peerName()
             << "connect!";
}

void MainWindow::newclient(clientInfo_s client)
{
    bool found = false;
    int idx = -1;
    Q_UNUSED(idx)
    for(int i=0;i<m_clientList.count();i++) {
        if(m_clientList.at(i).pSocket == client.pSocket) {
            found = true;
            idx = i;
            break;
        }
    }

    if(found) {
        m_clientList.append(client);
        QTableWidgetItem *clientItem = Q_NULLPTR;
        clientItem->setTextAlignment(Qt::AlignCenter);
        ui->tblTerminalList->setRowCount(ui->tblTerminalList->rowCount()+1);
        for(int i=0; i<ui->tblTerminalList->columnCount();i++) {
            clientItem = ui->tblTerminalList->item(ui->tblTerminalList->rowCount()-1, i);
            clientItem->setTextAlignment(Qt::AlignCenter);
            clientItem->setFlags(Qt::NoItemFlags);
        }
    } else {

    }
}

QString MainWindow::byteArrayToString(QByteArray buffer, bool read)
{
    QString buf;
    QString s;
    int i = 0;

    buf.clear();
    buf.append(QDateTime::currentDateTime().toString("[yyyy-MM-dd hh:mm:ss.zzz]"));
    buf += read?"[R:]":"[S:]";

    for (i = 0; i < buffer.count() - 1; i++) {
        s.clear();
        s.asprintf("%02X ", static_cast<unsigned char>(buffer.at(i)));
        buf += s;
    }

    s.clear();
    s.asprintf("%02X", static_cast<unsigned char>(buffer.at(i)));
    buf += s;
    s.clear();
    s.asprintf("\n");
    buf += s;

    return buf;
}

void MainWindow::socketReadData()
{
    bool isHex = ui->checkBox_RecvHex->isChecked();
    QByteArray buffer = pSocket->readAll();
    frm698_s frm698;
    memset(&frm698, 0, sizeof(frm698));

    if(!buffer.isEmpty()) {
        /*
         * 1. add data to queue of 698
         * 2. get a complete frame of 698 from queue
         * 3. decode frame
         * 4. analysis decode struct, do what frame ask
         * 5. if need response, then encode response frame
         *    and send it to relative terminal
         */
        u8 ret = decodeFrame((u8*)(buffer.data()), buffer.length(), &frm698);
        if(TRUE == ret) {
            qDebug() << "frame valid";
        } else {
            qDebug() << "frame invalid";
        }

        ui->textEdit_Recv->append(isHex ? byteArrayToString(buffer, true) : tr(buffer));
    }
}

void MainWindow::socketDisconnected()
{
    //发送按键失能
    ui->pushButton_Send->setEnabled(false);
    qDebug() << "Disconnected!";
}

