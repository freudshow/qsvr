#include "netconfigform.h"
#include "ui_netconfigform.h"

netConfigForm::netConfigForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::netConfigForm)
{
    ui->setupUi(this);
}

netConfigForm::~netConfigForm()
{
    delete ui;
}
