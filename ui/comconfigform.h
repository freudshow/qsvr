#ifndef COMCONFIGFORM_H
#define COMCONFIGFORM_H

#include <QWidget>

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
};

#endif // COMCONFIGFORM_H
