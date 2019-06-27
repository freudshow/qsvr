#ifndef SETLOGICADDR_H
#define SETLOGICADDR_H

#include <QDialog>

namespace Ui {
class setLogicAddr;
}

class setLogicAddr : public QDialog
{
    Q_OBJECT

public:
    explicit setLogicAddr(QWidget *parent = Q_NULLPTR);
    ~setLogicAddr();

private:
    Ui::setLogicAddr *ui;

private slots:
    void on_btnSet_clicked();
    void on_btnExit_clicked();
};

#endif // SETLOGICADDR_H
