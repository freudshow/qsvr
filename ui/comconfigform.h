#ifndef COMCONFIGFORM_H
#define COMCONFIGFORM_H

#include <QWidget>
#include <QSettings>
#include <QSerialPort>
#include <QSerialPortInfo>

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

    void on_btnSave_clicked();
    void on_btnExit_clicked();

signals:
    void exited();
};

#endif // COMCONFIGFORM_H
