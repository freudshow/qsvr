#ifndef NETCONFIGFORM_H
#define NETCONFIGFORM_H

#include <QWidget>
#include <QSettings>

namespace Ui {
class netConfigForm;
}

class netConfigForm : public QWidget
{
    Q_OBJECT

public:
    explicit netConfigForm(QWidget *parent = nullptr);
    ~netConfigForm();

private:
    Ui::netConfigForm *ui;
    QSettings *m_config;

private slots:
    void on_btnSave_clicked();
    void on_btnExit_clicked();

signals:
    void exited();
};

#endif // NETCONFIGFORM_H
