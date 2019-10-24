#include "update698.h"
#include "ui_update698.h"
#include <QFileDialog>
#include <QDir>

update698::update698(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::update698)
{
    ui->setupUi(this);
    connect(ui->btn_browse, SIGNAL(clicked(bool)), this, SLOT(browse()));
}

update698::~update698()
{
    delete ui;
}

void update698::on_btn_exit_clicked(bool)
{
    this->close();
}

void update698::browse()
{
    QString filename =  QFileDialog::getOpenFileName(this, tr("Open File"),
                                               QDir::homePath(),
                                               tr("all file (*)"));
    QFile file(filename);

    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, tr("open error"),
                              tr("can not open this file!"));
        return;
    }

    file.seek(0);
    m_filesize = file.size();
    m_fileblock = file.readAll();
    m_fileblock.mid(i, len);
    file.close();
}
