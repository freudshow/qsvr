#include "setlogicaddr.h"
#include "ui_setlogicaddr.h"

setLogicAddr::setLogicAddr(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::setLogicAddr)
{
    ui->setupUi(this);
}

setLogicAddr::~setLogicAddr()
{
    delete ui;
}

void setLogicAddr::on_btnSet_clicked()
{

}

void setLogicAddr::on_btnExit_clicked()
{
    this->close();
}
