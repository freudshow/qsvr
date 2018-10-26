#include "setrs485.h"
#include "ui_setrs485.h"

setRS485::setRS485(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::setRS485)
{
    ui->setupUi(this);
}

setRS485::~setRS485()
{
    delete ui;
}

void setRS485::on_btnSet_clicked()
{

}

void setRS485::on_btnExit_clicked()
{
    this->close();
}
