#ifndef COMCONFIGFORM_H
#define COMCONFIGFORM_H

#include <QWidget>
#include <QSettings>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QCloseEvent>


#define PARITY_NO	0
#define PARITY_ODD	1
#define PARITY_EVEN	2

#define STOP_ONE			0
#define STOP_ONEANDHALF     1
#define STOP_TWO			2

namespace Ui {
class comConfigForm;
}

class comConfigForm : public QWidget
{
    Q_OBJECT

public:
    explicit comConfigForm(QWidget *parent = nullptr);
    ~comConfigForm();

private:
    Ui::comConfigForm *ui;

    QSettings *m_comset;

private slots:
    void on_btnSave_clicked();
    void on_btnExit_clicked();

    void initCom();

    void closeEvent(QCloseEvent *event);

signals:
    void exited();
    void configFreshed();
    void configSaved();
};

#endif // COMCONFIGFORM_H
