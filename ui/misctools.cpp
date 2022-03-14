#include "misctools.h"
#include "ui_misctools.h"

#include "toolsdialog.h"
#include "filessum.h"

miscTools::miscTools(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::miscTools)
{
    ui->setupUi(this);

    toolsDialog *tool = new toolsDialog();
    ui->tabWidget->insertTab(0,tool,tr("check sum"));

    filesSum *file = new filesSum();
    ui->tabWidget->insertTab(0,file,tr("file sum"));
}

miscTools::~miscTools()
{
    delete ui;
}

void miscTools::on_miscTools_iconSizeChanged(const QSize &iconSize)
{

}

