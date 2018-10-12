#include "comconfigform.h"
#include "ui_comconfigform.h"

comConfigForm::comConfigForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::comConfigForm)
{
    ui->setupUi(this);
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        ui->portNameComboBox->addItem(info.portName());
    }

    //load default parameters
    m_comset = new QSettings("comdebugger.ini", QSettings::IniFormat);
    if (m_comset){
        ui->portNameComboBox->setCurrentIndex(m_comset->value("ComSettings/ComName").toInt());
        ui->baudRateComboBox->setCurrentIndex(m_comset->value("ComSettings/BaudRate").toInt());
        ui->dataBitsComboBox->setCurrentIndex(m_comset->value("ComSettings/DataBit").toInt());
        ui->parityComboBox->setCurrentIndex(m_comset->value("ComSettings/Parity").toInt());
        ui->stopBitsComboBox->setCurrentIndex(m_comset->value("ComSettings/StopBit").toInt());
    }
}

comConfigForm::~comConfigForm()
{
    delete ui;
}


void comConfigForm::on_btnSave_clicked()
{

}

void comConfigForm::on_btnExit_clicked()
{
    this->close();
    emit exited();
}
