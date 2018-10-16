#include <QMessageBox>
#include "netconfigform.h"
#include "ui_netconfigform.h"

netConfigForm::netConfigForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::netConfigForm)
{
    ui->setupUi(this);

    m_config = new QSettings("config/config.ini", QSettings::IniFormat);

    if (m_config) {
        ui->lineEditPort->setText(m_config->value("tcp/port").toString());
    }
}

netConfigForm::~netConfigForm()
{
    delete m_config;
    delete ui;
}

void netConfigForm::on_btnSave_clicked()
{
    m_config->setValue("tcp/port", ui->lineEditPort->text());
     QMessageBox::information(this, tr("Hint"), tr("save OK!"), QMessageBox::Ok);
}

void netConfigForm::on_btnExit_clicked()
{
    this->close();
}
