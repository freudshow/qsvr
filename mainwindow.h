#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>

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
	void server_New_Connect();
	void socket_Read_Data();
	void socket_Disconnected();

private:
	Ui::MainWindow *ui;
	QTcpServer* pServer;
	QTcpSocket* pSocket;
};

#endif // MAINWINDOW_H
