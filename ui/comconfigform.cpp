#include "comconfigform.h"
#include "ui_comconfigform.h"

comConfigForm::comConfigForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::comConfigForm)
{
    ui->setupUi(this);
}

comConfigForm::~comConfigForm()
{
    delete ui;
}
