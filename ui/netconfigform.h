#ifndef NETCONFIGFORM_H
#define NETCONFIGFORM_H

#include <QWidget>

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
};

#endif // NETCONFIGFORM_H
