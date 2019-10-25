#ifndef UPDATE698_H
#define UPDATE698_H

#include <QMainWindow>
#include <QString>
#include <QFile>
#include <QByteArray>
#include <QList>
#include <QSettings>

namespace Ui {
class update698;
}

class update698 : public QMainWindow
{
    Q_OBJECT

public:
    explicit update698(QWidget *parent = 0);
    ~update698();

private:
    Ui::update698 *ui;

    QList<QByteArray> m_blockList;
    QByteArray m_fileblock; //whole file bytes
    int m_filesize;      //whole file length
    int m_oneBlockSize;  //size of per file block
    int m_blockCount;    //counts of update file
    QSettings* m_settings;

private slots:
    void browse();
    void calcBlocks();
    void on_btn_exit_clicked(bool);
    void btnToggle(int);
};

#endif // UPDATE698_H
