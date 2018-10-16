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
    QList<QSerialPortInfo> portList = QSerialPortInfo::availablePorts();
    int count = portList.count();
    foreach (const QSerialPortInfo &info, portList) {
        ui->portNameComboBox->addItem(info.portName());
    }

    //load default parameters
    m_comset = new QSettings("config/config.ini", QSettings::IniFormat);

    if (m_comset) {
        for(int i = 0; i < ui->portNameComboBox->count(); i++) {
            if(ui->portNameComboBox->itemText(i) == m_comset->value("ComSettings/ComName").toString()) {
                ui->portNameComboBox->setCurrentIndex(i);
                break;
            }
        }

        for(int i = 0; i < ui->baudRateComboBox->count(); i++) {
            if(ui->baudRateComboBox->itemText(i).toInt() == m_comset->value("ComSettings/BaudRate").toInt()) {
                ui->baudRateComboBox->setCurrentIndex(i);
                break;
            }
        }

        for(int i = 0; i < ui->dataBitsComboBox->count(); i++) {
            if(ui->dataBitsComboBox->itemText(i).toInt() == m_comset->value("ComSettings/DataBit").toInt()) {
                ui->dataBitsComboBox->setCurrentIndex(i);
                break;
            }
        }

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
    m_comset->setValue("ComSettings/ComName", ui->portNameComboBox->currentText());
    m_comset->setValue("ComSettings/BaudRate", ui->baudRateComboBox->currentText());
    m_comset->setValue("ComSettings/DataBit", ui->dataBitsComboBox->currentText());
    m_comset->setValue("ComSettings/Parity", ui->parityComboBox->currentIndex());
    m_comset->setValue("ComSettings/StopBit", ui->stopBitsComboBox->currentIndex());

    emit configSaved();
    QMessageBox::information(this, tr("Hint"), tr("save OK!"), QMessageBox::Ok);
}

void comConfigForm::on_btnExit_clicked()
{
    this->close();
}
