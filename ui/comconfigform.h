#ifndef COMCONFIGFORM_H
#define COMCONFIGFORM_H

#include <QWidget>
#include <QSettings>
#include <QSerialPortInfo>
#include <QCloseEvent>

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
