#include "filessum.h"
#include "ui_filessum.h"
#include "libutil/md5.h"

#include <QFile>
#include <QDebug>
#include <QFileDialog>
#include <QCloseEvent>
#include <QShowEvent>
#include <QResizeEvent>
#include <QCryptographicHash>

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
    calcSum();
}

void filesSum::on_btn_exit_clicked()
{
    this->close();
}

void filesSum::closeEvent(QCloseEvent *event)
{
    int button = QMessageBox::question(this, tr("Question"), tr("Exit Really?"), \
        QMessageBox::Yes|QMessageBox::Default, QMessageBox::No|QMessageBox::Escape);
    if (button == QMessageBox::No)
    {
        return event->ignore();
    }
    else
    {
        emit fileClose();
        event->accept();
    }
}

void filesSum::showEvent(QShowEvent * event)
{
    Q_UNUSED(event);
}

void filesSum::resizeEvent(QResizeEvent * event)
{
    Q_UNUSED(event);
}

void filesSum::on_btn_calc_clicked()
{
    int idx = ui->rbtnGroup_sum->checkedId();
    m_sumFileMethod = static_cast<sumFileMethod_e>(idx);
    calcSum();
}

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

u16 filesSum::crcfile(QString filename)
{
    uint16_t crc = 0;
    uint32_t offset = 0;
    uint32_t readLen = 0;
    uint32_t fileSize = 0;
    uint32_t count = 0;
    uint32_t i = 0;
    QFile file(filename);
    QByteArray b;

    fileSize = file.size();
    count = fileSize / 1024 + (((fileSize % 1024) == 0) ? 0 : 1);
    for (i = 0, offset = 0; i < count; i++, offset += readLen) {
        file.seek(offset);
        b = file.read(1024);
        crc = crc16continue((u8*)b.data(), b.length(), crc, i);
    }

    return crc;
}

void filesSum::sumFileMd5()
{
    QFile file(ui->lineEdit_filename->text());
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "read file failed!";
        return;
    }

    QByteArray b = file.readAll();
    QCryptographicHash hash(QCryptographicHash::Md5);
    hash.addData(b);
    QByteArray result = hash.result();
    ui->lineEdit_result->setText(QString(result.toHex()));
}

void filesSum::sumFileSha1()
{
    QFile file(ui->lineEdit_filename->text());
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "read file failed!";
        return;
    }

    QByteArray b = file.readAll();
    QCryptographicHash hash(QCryptographicHash::Sha1);
    hash.addData(b);
    QByteArray result = hash.result();
    ui->lineEdit_result->setText(QString(result.toHex()));
}

void filesSum::sumFileSha256()
{
    QFile file(ui->lineEdit_filename->text());
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "read file failed!";
        return;
    }

    QByteArray b = file.readAll();
    QCryptographicHash hash(QCryptographicHash::Sha256);
    hash.addData(b);
    QByteArray result = hash.result();
    ui->lineEdit_result->setText(QString(result.toHex()));
}
