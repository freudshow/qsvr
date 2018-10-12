#include <QMessageBox>
#include "comconfigform.h"
#include "ui_comconfigform.h"

comConfigForm::comConfigForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::comConfigForm)
{
    ui->setupUi(this);
    initCom();
}

void comConfigForm::closeEvent(QCloseEvent *event)
{
    emit exited();
}

void comConfigForm::initCom()
{
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        ui->portNameComboBox->addItem(info.portName());
    }

    //load default parameters
    m_comset = new QSettings("config/config.ini", QSettings::IniFormat);

    if (m_comset) {
        ui->portNameComboBox->setCurrentIndex(m_comset->value("ComSettings/ComName").toInt());
        ui->baudRateComboBox->setCurrentIndex(m_comset->value("ComSettings/BaudRate").toInt());
        ui->dataBitsComboBox->setCurrentIndex(m_comset->value("ComSettings/DataBit").toInt());
        ui->parityComboBox->setCurrentIndex(m_comset->value("ComSettings/Parity").toInt());
        ui->stopBitsComboBox->setCurrentIndex(m_comset->value("ComSettings/StopBit").toInt());
    }
}

comConfigForm::~comConfigForm()
{
    delete m_comset;
    delete ui;
}

void comConfigForm::on_btnSave_clicked()
{
    m_comset->setValue("ComSettings/ComName", ui->portNameComboBox->currentIndex());
    m_comset->setValue("ComSettings/BaudRate", ui->baudRateComboBox->currentIndex());
    m_comset->setValue("ComSettings/DataBit", ui->dataBitsComboBox->currentIndex());
    m_comset->setValue("ComSettings/Parity", ui->parityComboBox->currentIndex());
    m_comset->setValue("ComSettings/StopBit", ui->stopBitsComboBox->currentIndex());

    emit configSaved();
    QMessageBox::information(this, tr("Hint"), tr("save OK!"), QMessageBox::Ok);
}

void comConfigForm::on_btnExit_clicked()
{
    this->close();
}
