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

unsigned char buf[] = {0x68, 0x1b, 0x00, 0x43, 0x03, 0x07, 0x00, 0x00, 0x00, 0x10, 0xf6, 0xa4, 0x06, 0x01, 0x00, 0x40, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0x08, 0x00, 0xe4, 0x41, 0x16};
    QByteArray sendbuf;
    sendbuf.append(buf[0]);

}

void setLogicAddr::on_btnExit_clicked()
{
    this->close();
}
