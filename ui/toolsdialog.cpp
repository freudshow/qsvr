#include <QRegExp>

#include "toolsdialog.h"
#include "ui_toolsdialog.h"

toolsDialog::toolsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::toolsDialog)
{
    ui->setupUi(this);
    QObject::connect(ui->text_input, SIGNAL(textChanged()), this, SLOT(prepareText()));

    m_calcMethod = e_calcNothing;

    ui->rbtnGroup_calc->setId(ui->rbtn_crc16, e_calcCRC16);
    ui->rbtnGroup_calc->setId(ui->rbtn_fcs, e_calcFCS);
    ui->rbtnGroup_calc->setId(ui->rbtn_sum, e_calcSumChk);
    ui->rbtnGroup_calc->setId(ui->rbtn_add0x33, e_calcAdd33);
    ui->rbtnGroup_calc->setId(ui->rbtn_minus0x33, e_calcMinus33);
    ui->rbtnGroup_calc->setId(ui->rbtn_inverse, e_calcInverse);
    QObject::connect(ui->rbtnGroup_calc, SIGNAL(buttonClicked(int)), this, SLOT(btnToggle(int)));
}

toolsDialog::~toolsDialog()
{
    delete ui;
}

void toolsDialog::prepareText()
{
    m_str = ui->text_input->toPlainText();
    m_str.remove(QRegExp("\\s")).remove(QRegExp("\\t")).remove(QRegExp("\\n"));
    QObject::disconnect(ui->text_input, SIGNAL(textChanged()), this, SLOT(prepareText()));
    ui->text_input->setText(m_str);
    QObject::connect(ui->text_input, SIGNAL(textChanged()), this, SLOT(prepareText()));

    QMessageBox::information(this, tr("err"), m_str, QMessageBox::Ok);
}

void toolsDialog::on_text_input_textChanged()
{

}

void toolsDialog::on_btn_calc_clicked()
{

}

void toolsDialog::on_btn_exit_clicked()
{
    emit toolExit();
}

void toolsDialog::btnToggle(int idx)
{


    ui->rbtnGroup_calc->buttons().at(idx);
}

