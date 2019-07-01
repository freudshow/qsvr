#include <QRegExp>
#include <QClipboard>
#include "toolsdialog.h"
#include "ui_toolsdialog.h"

toolsDialog::toolsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::toolsDialog)
{
    ui->setupUi(this);
    ui->label_formatError->setText("");
    QObject::connect(ui->text_input, SIGNAL(textChanged()), this, SLOT(prepareText()));

    m_calcMethod = e_calcNothing;

    m_map.clear();
    m_map[e_calcCRC16] = &toolsDialog::calcCRC16;
    m_map[e_calcFCS] = &toolsDialog::calcFCS;
    m_map[e_calcSumChk] = &toolsDialog::calcSumChk;
    m_map[e_calcAdd33] = &toolsDialog::calcAdd33;
    m_map[e_calcMinus33] = &toolsDialog::calcMinus33;
    m_map[e_calcInverse] = &toolsDialog::calcInverse;

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
    QRegExp re("[\\x0-9a-fA-F]+");
    QStringList list;
    QString s;

    s = ui->text_input->toPlainText().remove(QRegExp("\\n")).
                                      remove(QRegExp("\\r")).
                                      remove(QRegExp("\\s")).
                                      remove(QRegExp("\\t"));

    if(re.exactMatch(s) && (s.count()%2) == 0) {
        ui->label_formatError->setText("");
        QObject::disconnect(ui->text_input, SIGNAL(textChanged()), this, SLOT(prepareText()));
        QString t;
        for (int i = 0; i<s.count();i+=2) {
              t.append(s.at(i)).append(s.at(i+1)).append(" ");
        }
        ui->text_input->setText(t.trimmed());
        QObject::connect(ui->text_input, SIGNAL(textChanged()), this, SLOT(prepareText()));
    } else {
        ui->label_formatError->setText("<font color=Red><b>Input Format Error!</b></font>");
    }
}

void toolsDialog::on_btn_calc_clicked()
{
    if(m_calcMethod > e_calcNothing)
        (this->*m_map[m_calcMethod])();
}

void toolsDialog::on_btn_copy_clicked()
{
    QClipboard* cp = QApplication::clipboard();
    cp->setText(ui->lineEdit_result->text());
}

void toolsDialog::on_btn_exit_clicked()
{
    emit toolExit();
}

void toolsDialog::btnToggle(int idx)
{
    m_calcMethod = static_cast<calcMethod_e>(idx);
}

QString toolsDialog::byteArrayToString(QByteArray buffer)
{
    QString buf;
    QString s;
    int i = 0;

    buf.clear();

    for (i = 0; i < buffer.count() - 1; i++) {
        s.clear();
        s.sprintf("%02X ", static_cast<unsigned char>(buffer.at(i)));
        buf += s;
    }

    s.clear();
    s.sprintf("%02X", static_cast<unsigned char>(buffer.at(i)));
    buf += s;

    return buf;
}

void toolsDialog::calcCRC16()
{
    QString s;
    QByteArray b;
    QStringList l;
    bool ok;

    b.clear();
    l.clear();

    l = ui->text_input->toPlainText().split(" ");

    for (int i = 0; i< l.count();i++) {
        b.append((char)l.at(i).toInt(&ok, 16));
    }

    u16 crc = crc16TblDrv((u8*)b.data(), b.length());
    s.clear();
    s.sprintf("%02X %02X", (u8)crc, (u8)(crc>>8));
    ui->lineEdit_result->setText(s);
}

void toolsDialog::calcFCS()
{
    QString s;
    QByteArray b;
    QStringList l;
    bool ok;

    b.clear();
    l.clear();

    l = ui->text_input->toPlainText().split(" ");

    for (int i = 0; i< l.count();i++) {
        b.append((char)l.at(i).toInt(&ok, 16));
    }

    u16 crc = fcs16((u8*)b.data(), b.length());
    s.clear();
    s.sprintf("%02X %02X", (u8)crc, (u8)(crc>>8));
    ui->lineEdit_result->setText(s);
}

void toolsDialog::calcSumChk()
{
    QString s;
    QByteArray b;
    QStringList l;
    bool ok;

    b.clear();
    l.clear();

    l = ui->text_input->toPlainText().split(" ");

    for (int i = 0; i< l.count();i++) {
        b.append((char)l.at(i).toInt(&ok, 16));
    }

    u8 crc = chkSum((u8*)b.data(), b.length());
    s.clear();
    s.sprintf("%02X", crc);
    ui->lineEdit_result->setText(s);
}

void toolsDialog::calcAdd33()
{
    QByteArray b;
    QStringList l;
    bool ok;

    b.clear();
    l.clear();

    l = ui->text_input->toPlainText().split(" ");

    for (int i = 0; i< l.count();i++) {
        b.append((char)l.at(i).toInt(&ok, 16)+0x33);
    }

//    ui->lineEdit_result->setText(b.toHex(' '));
    ui->lineEdit_result->setText(byteArrayToString(b));//qt 5.5 does not have QByteArray.toHex(char separator) const
}

void toolsDialog::calcMinus33()
{
    QByteArray b;
    QStringList l;
    bool ok;

    b.clear();
    l.clear();

    l = ui->text_input->toPlainText().split(" ");

    for (int i = 0; i< l.count();i++) {
        b.append((char)l.at(i).toInt(&ok, 16)-0x33);
    }

    ui->lineEdit_result->setText(byteArrayToString(b));
}

void toolsDialog::calcInverse()
{
    QByteArray b;
    QStringList l;
    int len = 0;
    bool ok;

    b.clear();
    l.clear();

    l = ui->text_input->toPlainText().split(" ");

    len = l.count();
    for (int i = 0; i< len;i++) {
        b.append((char)l.at(len-1-i).toInt(&ok, 16));
    }

    ui->lineEdit_result->setText(byteArrayToString(b));
}
