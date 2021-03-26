#include "mainwindow.h"
#include "update698.h"
#include "ui/toolsdialog.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QString binPath = QApplication::applicationDirPath();

    MainWindow w;
    w.show();

//    update698 u;
//    u.show();

//    toolsDialog t;
//    t.show();

    return a.exec();
}
