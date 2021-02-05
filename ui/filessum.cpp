#include "filessum.h"
#include "ui_filessum.h"

#include <QFile>
#include <QDebug>
#include <QFileDialog>

filesSum::filesSum(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::filesSum)
{
    ui->setupUi(this);

    m_sumFileMethod = e_sumFileNothing;

    ui->rbtnGroup_sum->setId(ui->rbtn_crc16, e_sumFileCRC16);
    ui->rbtnGroup_sum->setId(ui->rbtn_md5, e_sumFileMd5);
    ui->rbtnGroup_sum->setId(ui->rbtn_sha1, e_sumFileSha1);
    ui->rbtnGroup_sum->setId(ui->rbtn_sha256, e_sumFileSha256);

    m_map.clear();
    m_map[e_sumFileCRC16] = &filesSum::sumFileCrc16;
    m_map[e_sumFileMd5] = &filesSum::sumFileMd5;
    m_map[e_sumFileSha1] = &filesSum::sumFileSha1;
    m_map[e_sumFileSha256] = &filesSum::sumFileSha256;

    QObject::connect(ui->rbtnGroup_sum, SIGNAL(buttonClicked(int)), this, SLOT(btnToggle(int)));

    this->setAttribute(Qt::WA_DeleteOnClose, true);
}

filesSum::~filesSum()
{
    delete ui;
}

void filesSum::btnToggle(int idx)
{
    m_sumFileMethod = static_cast<sumFileMethod_e>(idx);
    calcSum();
}

void filesSum::on_btn_choose_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("请选择文件"),
                                                    "D:/",
                                                    tr("*"));

    ui->lineEdit_filename->setText(fileName);
}

void filesSum::on_btn_exit_clicked()
{
    this->close();
    emit fileClose();
}

void filesSum::on_btn_calc_clicked() {}

void filesSum::calcSum()
{
    if (m_sumFileMethod > e_sumFileNothing)
        (this->*m_map[m_sumFileMethod])();
}

void filesSum::sumFileCrc16()
{
    QFile file(ui->lineEdit_filename->text());
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "read file failed!";
        return;
    }

    QByteArray b = file.readAll();
    char *p = b.data();

    u16 crc = crc16TblDrv((u8 *) p, b.length());

    ui->lineEdit_result->setText(QString::number(crc, 16));
}

void filesSum::sumFileMd5()
{

}

void filesSum::sumFileSha1()
{

}

void filesSum::sumFileSha256()
{

}
