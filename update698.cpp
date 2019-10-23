#include "update698.h"
#include "ui_update698.h"

update698::update698(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::update698)
{
    ui->setupUi(this);
}

update698::~update698()
{
    delete ui;
}
