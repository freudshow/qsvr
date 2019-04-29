#include "toolsdialog.h"
#include "ui_toolsdialog.h"

toolsDialog::toolsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::toolsDialog)
{
    ui->setupUi(this);

    m_calcMethod = e_calcNothing;

    ui->rbtnGroup_calc->setId(ui->rbtn_crc16, 0);
    ui->rbtnGroup_calc->setId(ui->rbtn_fcs, 1);
    ui->rbtnGroup_calc->setId(ui->rbtn_sum, 2);
    ui->rbtnGroup_calc->setId(ui->rbtn_add0x33, 3);
    ui->rbtnGroup_calc->setId(ui->rbtn_minus0x33, 4);
    ui->rbtnGroup_calc->setId(ui->rbtn_inverse, 5);
    QObject::connect(ui->rbtnGroup_calc, SIGNAL(buttonClicked(int)), this, SLOT(btnToggle(int)));
}

toolsDialog::~toolsDialog()
{
    delete ui;
}

void toolsDialog::prepareText()
{
    m_str = ui->text_input->toPlainText();
    QMessageBox::information(this, tr("err"), tr("format error"), QMessageBox::Ok);
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

