#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	ui->lineEdit_Port->setText("8002");
	ui->pushButton_Send->setEnabled(false);

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
		//从输入框获取端口号
		int port = ui->lineEdit_Port->text().toInt();

		//监听指定的端口
        if(!pServer->listen(QHostAddress::Any, port)){
			//若出错，则输出错误信息
			qDebug()<<pServer->errorString();
			return;
		}
		//修改按键文字
		ui->pushButton_Listen->setText("取消侦听");
		qDebug()<< "Listen succeessfully!";
        listenState = !listenState;
    } else {
		//如果正在连接（点击侦听后立即取消侦听，若socket没有指定对象会有异常，应修正——2017.9.30）
        if (pSocket->state() == QAbstractSocket::ConnectedState) {
			//关闭连接
			pSocket->disconnectFromHost();
		}
		//取消侦听
		pServer->close();
		//修改按键文字
		ui->pushButton_Listen->setText("侦听");
		//发送按键失能
		ui->pushButton_Send->setEnabled(false);
	}
}

void MainWindow::on_pushButton_Send_clicked()
{
	qDebug() << "Send: " << ui->textEdit_Send->toPlainText();
	//获取文本框内容并以ASCII码形式发送
	pSocket->write(ui->textEdit_Send->toPlainText().toLatin1());
	pSocket->flush();
}

void MainWindow::serverNewConnect()
{
	//获取客户端连接
	pSocket = pServer->nextPendingConnection();
	//连接QTcpSocket的信号槽，以读取新数据
    QObject::connect(pSocket, &QTcpSocket::readyRead, this, &MainWindow::socketReadData);
    QObject::connect(pSocket, &QTcpSocket::disconnected, this, &MainWindow::socketDisconnected);
	//发送按键使能
	ui->pushButton_Send->setEnabled(true);

	qDebug() << "A Client connect!";
}

void MainWindow::socketReadData()
{
	QByteArray buffer;
	//读取缓冲区数据
	buffer = pSocket->readAll();
	if(!buffer.isEmpty())
	{
		QString str = ui->textEdit_Recv->toPlainText();
		str+=tr(buffer);
		//刷新显示
		ui->textEdit_Recv->setText(str);
	}
}

void MainWindow::socketDisconnected()
{
	//发送按键失能
	ui->pushButton_Send->setEnabled(false);
	qDebug() << "Disconnected!";
}
