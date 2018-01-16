#include <QMessageBox>
#include <QDateTime>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "lib698/lib698.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	ui->lineEdit_Port->setText("8002");
	ui->pushButton_Send->setEnabled(false);
    ui->checkBox_RecvHex->setChecked(true);
    ui->checkBox_SendHex->setChecked(true);

	pServer = new QTcpServer();

	//连接信号槽
    connect(pServer,&QTcpServer::newConnection,this,&MainWindow::serverNewConnect);

    listenState = false;
}

MainWindow::~MainWindow()
{
	pServer->close();
	pServer->deleteLater();
	delete ui;
}

void MainWindow::on_pushButton_Listen_clicked()
{
    if(!listenState) {
		int port = ui->lineEdit_Port->text().toInt();

        if(!pServer->listen(QHostAddress::Any, port)){
            qDebug() << pServer->errorString();
			return;
		}

        ui->pushButton_Listen->setText(tr("Don't Listen"));
		qDebug()<< "Listen succeessfully!";
    } else if(NULL != pSocket) {
		//如果正在连接（点击侦听后立即取消侦听，若socket没有指定对象会有异常，应修正——2017.9.30）
        if (pSocket->state() == QAbstractSocket::ConnectedState) {
			//关闭连接
			pSocket->disconnectFromHost();
		}
		//取消侦听
		pServer->close();
        ui->pushButton_Listen->setText(tr("Listen"));
		ui->pushButton_Send->setEnabled(false);
	}
    listenState = !listenState;
}

void MainWindow::on_pushButton_Send_clicked()
{
    sendMsg();
	qDebug() << "Send: " << ui->textEdit_Send->toPlainText();
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
            data = (char)list.at(i).toInt(&ok, 16);
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

    pSocket->write(buf);
    pSocket->flush();

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

    qDebug() << "A Client connect!";
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
        s.sprintf("%02X ", (unsigned char)buffer.at(i));
        buf += s;
    }

    s.clear();
    s.sprintf("%02X", (unsigned char)buffer.at(i));
    buf += s;
    s.clear();
    s.sprintf("\n");
    buf += s;

    return buf;
}

void MainWindow::socketReadData()
{
    bool isHex = ui->checkBox_RecvHex->isChecked();
    QByteArray buffer = pSocket->readAll();

    if(!buffer.isEmpty()) {
        u8 ret = processFrame((u8*)(buffer.data()), buffer.length());
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

