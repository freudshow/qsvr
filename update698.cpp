#include "update698.h"
#include "ui_update698.h"
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>
#include <QSettings>

update698::update698(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::update698),
    m_settings(new QSettings("config/config.ini", QSettings::IniFormat, this))
{
    ui->setupUi(this);
    connect(ui->btn_browse, SIGNAL(clicked(bool)), this, SLOT(browse()));

    int size = m_settings->value("fileBlocks/defaultSize").toInt();
    m_oneBlockSize = (size > 0 ? size : 512);
    ui->buttonGroup->setId(ui->rbtn_256, 0);
    ui->buttonGroup->setId(ui->rbtn_512, 1);
    ui->buttonGroup->setId(ui->rbtn_1024, 2);
    QObject::connect(ui->buttonGroup, SIGNAL(buttonClicked(int)), this, SLOT(btnToggle(int)));
    ui->buttonGroup->button(m_settings->value("fileBlocks/rbtnIndex").toInt()%3)->setChecked(true);
}

update698::~update698()
{
    m_settings->setValue("fileBlocks/rbtnIndex", ui->buttonGroup->checkedId());
    delete ui;
}

void update698::on_btn_exit_clicked(bool)
{
    this->close();
}

void update698::btnToggle(int i)
{
    switch (i) {
    case 0:
        m_oneBlockSize = 256;
        break;
    case 1:
        m_oneBlockSize = 512;
        break;
    case 2:
        m_oneBlockSize = 1024;
        break;
    default:
        m_oneBlockSize = 512;
        break;
    }
}

void update698::browse()
{
    QString filename =  QFileDialog::getOpenFileName(this, tr("Open File"),
                                               QDir::homePath(),
                                               tr("all file (*)"));
    ui->lineEdit_filename->setText(filename);
    QFile file(filename);

    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, tr("open error"),
                              tr("can not open this file!"));
        return;
    }

    file.seek(0);
    m_fileblock = file.readAll();
    file.close();
    calcBlocks();
}

void update698::calcBlocks()
{
    int i = 0;

    m_filesize = m_fileblock.count();
    m_blockCount = m_filesize/m_oneBlockSize + (((m_filesize%m_oneBlockSize)==0) ? 0 : 1);
    m_blockList.clear();

    for (i = 0; (i + m_oneBlockSize) <= m_filesize; i += m_oneBlockSize)
        m_blockList.append(m_fileblock.mid(i, m_oneBlockSize));

    if ((m_filesize-i) > 0)
        m_blockList.append(m_fileblock.mid(i, (m_filesize-i)));
}
