#ifndef SETRS485_H
#define SETRS485_H

#include <QDialog>

namespace Ui {
class setRS485;
}

class setRS485 : public QDialog
{
    Q_OBJECT

public:
    explicit setRS485(QWidget *parent = nullptr);
    ~setRS485();

private:
    Ui::setRS485 *ui;

private slots:
    void on_btnSet_clicked();
    void on_btnExit_clicked();
};

#endif // SETRS485_H
