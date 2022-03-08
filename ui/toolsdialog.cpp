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
    m_map[e_ascii] = &toolsDialog::calcAscii;
    m_map[e_float] = &toolsDialog::calcFloat;
    m_map[e_doubleFloat] = &toolsDialog::calcDoubleFloat;
    m_map[e_xorsum] = &toolsDialog::calcXorsum;
    m_map[e_uint32] = &toolsDialog::calcUInt32;
    m_map[e_uint16] = &toolsDialog::calcUInt16;
    m_map[e_int32] = &toolsDialog::calcInt32;
    m_map[e_int16] = &toolsDialog::calcInt16;

    ui->rbtnGroup_calc->setId(ui->rbtn_crc16, e_calcCRC16);
    ui->rbtnGroup_calc->setId(ui->rbtn_fcs, e_calcFCS);
    ui->rbtnGroup_calc->setId(ui->rbtn_sum, e_calcSumChk);
    ui->rbtnGroup_calc->setId(ui->rbtn_add0x33, e_calcAdd33);
    ui->rbtnGroup_calc->setId(ui->rbtn_minus0x33, e_calcMinus33);
    ui->rbtnGroup_calc->setId(ui->rbtn_inverse, e_calcInverse);
    ui->rbtnGroup_calc->setId(ui->rbtn_ascii, e_ascii);
    ui->rbtnGroup_calc->setId(ui->rbtn_float, e_float);
    ui->rbtnGroup_calc->setId(ui->rbtn_doubleFloat, e_doubleFloat);
    ui->rbtnGroup_calc->setId(ui->rbtn_xorsum, e_xorsum);
    ui->rbtnGroup_calc->setId(ui->rbtn_uint32, e_uint32);
    ui->rbtnGroup_calc->setId(ui->rbtn_uint16, e_uint16);
    ui->rbtnGroup_calc->setId(ui->rbtn_int32, e_int32);
    ui->rbtnGroup_calc->setId(ui->rbtn_int16, e_int16);

    QObject::connect(ui->rbtnGroup_calc, SIGNAL(buttonClicked(int)), this, SLOT(btnToggle(int)));

    this->setAttribute(Qt::WA_DeleteOnClose, true);
}

toolsDialog::~toolsDialog()
{
    delete ui;
}

void toolsDialog::prepareText()
{
    QRegExp re("[\\x0-9a-fA-F]+");
    QString s;

    s = ui->text_input->toPlainText()
            .remove(QRegExp("\\n"))
            .remove(QRegExp("\\r"))
            .remove(QRegExp("\\s"))
            .remove(QRegExp("\\t"));

    if(re.exactMatch(s) && (s.count()%2) == 0) {
        ui->label_formatError->setText("");

        QString t;

        int i = 0;
        for (i = 0; i < s.count(); i += 2)
            t.append(s.at(i)).append(s.at(i + 1)).append(" ");

        QObject::disconnect(ui->text_input, SIGNAL(textChanged()), this, SLOT(prepareText()));
        ui->text_input->setText(t.trimmed());
        QObject::connect(ui->text_input, SIGNAL(textChanged()), this, SLOT(prepareText()));
        m_nBytes = i / 2;
        ui->label_formatError->setText(QString::number(m_nBytes) + tr(" bytes"));
        calcSums();
    } else {
        ui->label_formatError->setText("<font color=Red><b>Input Format Error!</b></font>");
    }
}

void toolsDialog::on_lineEdit_result_textChanged()
{
    QByteArray b;
    bool ok;

    if (e_float == m_calcMethod) {
        QObject::disconnect(ui->text_input, SIGNAL(textChanged()), this, SLOT(prepareText()));

        float f = ui->lineEdit_result->text().toFloat(&ok);
        if (ok) {
            char *p = (char *) &f;
            for (u32 i = 0; i < sizeof(f); i++)
                b.append(p[i]);
        }

        ui->text_input->setText(b.toHex(' '));
        QObject::connect(ui->text_input, SIGNAL(textChanged()), this, SLOT(prepareText()));
    } else if (e_doubleFloat == m_calcMethod) {
        QObject::disconnect(ui->text_input, SIGNAL(textChanged()), this, SLOT(prepareText()));

        double f = ui->lineEdit_result->text().toDouble(&ok);
        if (ok) {
            char *p = (char *) &f;
            for (u32 i = 0; i < sizeof(f); i++)
                b.append(p[i]);
        }

        ui->text_input->setText(b.toHex(' '));
        QObject::connect(ui->text_input, SIGNAL(textChanged()), this, SLOT(prepareText()));
    }
}

void toolsDialog::calcSums()
{
    if(m_calcMethod > e_calcNothing)
        (this->*m_map[m_calcMethod])();
}

void toolsDialog::on_btn_calc_clicked()
{
    calcSums();
}

void toolsDialog::on_btn_copy_clicked()
{
    QClipboard* cp = QApplication::clipboard();
    cp->setText(ui->lineEdit_result->text());
}

void toolsDialog::on_btn_exit_clicked()
{
    this->close();
    emit toolExit();
}

void toolsDialog::btnToggle(int idx)
{
    m_calcMethod = static_cast<calcMethod_e>(idx);
    calcSums();
}

QString toolsDialog::byteArrayToString(QByteArray buffer)
{
    QString buf;
    QString s;
    int i = 0;

    buf.clear();

    for (i = 0; i < buffer.count() - 1; i++) {
        s.clear();
        s.asprintf("%02X ", static_cast<unsigned char>(buffer.at(i)));
        buf += s;
    }

    s.clear();
    s.asprintf("%02X", static_cast<unsigned char>(buffer.at(i)));
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
    s.append(QString("%1 %2").arg(crc & 0xFF, 2, 16, QLatin1Char('0')).arg((u8)(crc >> 8), 2, 16, QLatin1Char('0')));
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
    s.append(QString("%1 %2").arg(crc & 0xFF, 2, 16, QLatin1Char('0')).arg((u8)(crc >> 8), 2, 16, QLatin1Char('0')));
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
    s.asprintf("%02X", crc);
    s.append(QString("%1").arg(crc & 0xFF, 2, 16, QLatin1Char('0')));
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

    ui->lineEdit_result->setText(b.toHex(' '));
//    ui->lineEdit_result->setText(byteArrayToString(b));//qt 5.5 does not have QByteArray.toHex(char separator) const
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

    ui->lineEdit_result->setText(b.toHex(' '));
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

    ui->lineEdit_result->setText(b.toHex(' '));
}

void toolsDialog::calcAscii()
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
        b.append((char)l.at(i).toInt(&ok, 16));
    }

    QString s = QString(b);

    ui->lineEdit_result->setText(QString(b));
}

void toolsDialog::calcFloat()
{
    QByteArray b;
    QStringList l;
    int len = 0;
    bool ok;
    float f = 0.0;

    b.clear();
    l.clear();

    l = ui->text_input->toPlainText().split(" ");

    len = l.count();
    if (len != sizeof(f)) {
        ui->label_formatError->setText("<font color=Red><b>Input Format Error!</b></font>");
        return;
    }

    ui->label_formatError->setText(QString::number(m_nBytes) + tr(" bytes"));

    for (int i = 0; i < len; i++) {
        b.append((char) l.at(i).toInt(&ok, 16));
    }

    memcpy(&f, b.data(), sizeof(f));
    ui->lineEdit_result->setText(QString::number(f));
}

void toolsDialog::calcDoubleFloat()
{
    QByteArray b;
    QStringList l;
    int len = 0;
    bool ok;
    double f = 0.0;

    b.clear();
    l.clear();

    l = ui->text_input->toPlainText().split(" ");

    len = l.count();
    if (len != sizeof(f)) {
        ui->label_formatError->setText("<font color=Red><b>Input Format Error!</b></font>");
        return;
    }

    ui->label_formatError->setText(QString::number(m_nBytes) + tr(" bytes"));

    for (int i = 0; i < len; i++) {
        b.append((char) l.at(i).toInt(&ok, 16));
    }

    memcpy(&f, b.data(), sizeof(f));
    ui->lineEdit_result->setText(QString::number(f));
}

void toolsDialog::calcXorsum()
{
    QByteArray b;
    QStringList l;
    int len = 0;
    bool ok;

    b.clear();
    l.clear();

    l = ui->text_input->toPlainText().split(" ");

    len = l.count();
    for (int i = 0; i < len;i++) {
        b.append((char)l.at(i).toInt(&ok, 16));
    }

    u8 x = xorSum((u8*)b.data(), b.length());
    ui->lineEdit_result->setText(QString("%1").arg(x, 2, 16, QLatin1Char('0')));
}

void toolsDialog::calcUInt32()
{
    QByteArray b;
    QStringList l;
    int len = 0;
    bool ok;
    uint32_t u = 0;

    b.clear();
    l.clear();

    l = ui->text_input->toPlainText().split(" ");

    len = l.count();
    if (len != sizeof(u)) {
        ui->label_formatError->setText("<font color=Red><b>Input Format Error!</b></font>");
        return;
    }

    ui->label_formatError->setText(QString::number(m_nBytes) + tr(" bytes"));

    for (int i = 0; i < len; i++) {
        b.append((char) l.at(i).toInt(&ok, 16));
    }

    memcpy(&u, b.data(), sizeof(u));
    ui->lineEdit_result->setText(QString::number(u));
}

void toolsDialog::calcUInt16()
{
    QByteArray b;
    QStringList l;
    int len = 0;
    bool ok;
    uint16_t u = 0;

    b.clear();
    l.clear();

    l = ui->text_input->toPlainText().split(" ");

    len = l.count();
    if (len != sizeof(u)) {
        ui->label_formatError->setText("<font color=Red><b>Input Format Error!</b></font>");
        return;
    }

    ui->label_formatError->setText(QString::number(m_nBytes) + tr(" bytes"));

    for (int i = 0; i < len; i++) {
        b.append((char) l.at(i).toInt(&ok, 16));
    }

    memcpy(&u, b.data(), sizeof(u));
    ui->lineEdit_result->setText(QString::number(u));
}

void toolsDialog::calcInt32()
{
    QByteArray b;
    QStringList l;
    int len = 0;
    bool ok;
    int32_t u = 0;

    b.clear();
    l.clear();

    l = ui->text_input->toPlainText().split(" ");

    len = l.count();
    if (len != sizeof(u)) {
        ui->label_formatError->setText("<font color=Red><b>Input Format Error!</b></font>");
        return;
    }

    ui->label_formatError->setText(QString::number(m_nBytes) + tr(" bytes"));

    for (int i = 0; i < len; i++) {
        b.append((char) l.at(i).toInt(&ok, 16));
    }

    memcpy(&u, b.data(), sizeof(u));
    ui->lineEdit_result->setText(QString::number(u));
}

void toolsDialog::calcInt16()
{
    QByteArray b;
    QStringList l;
    int len = 0;
    bool ok;
    int16_t u = 0;

    b.clear();
    l.clear();

    l = ui->text_input->toPlainText().split(" ");

    len = l.count();
    if (len != sizeof(u)) {
        ui->label_formatError->setText("<font color=Red><b>Input Format Error!</b></font>");
        return;
    }

    ui->label_formatError->setText(QString::number(m_nBytes) + tr(" bytes"));

    for (int i = 0; i < len; i++) {
        b.append((char) l.at(i).toInt(&ok, 16));
    }

    memcpy(&u, b.data(), sizeof(u));
    ui->lineEdit_result->setText(QString::number(u));
}
