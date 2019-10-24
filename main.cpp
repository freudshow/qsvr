#include "mainwindow.h"
#include "update698.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    MainWindow w;
//    w.show();

    update698 u;
    u.show();

    return a.exec();
}
