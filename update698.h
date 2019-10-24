#ifndef UPDATE698_H
#define UPDATE698_H

#include <QMainWindow>
#include <QString>
#include <QFile>
#include <QByteArray>
#include <QList>

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
    qint64 m_filesize;      //whole file length
    qint64 m_oneBlockSize;  //size of per file block
    qint64 m_blockCount;    //counts of update file

private slots:
    void browse();
    void on_btn_exit_clicked(bool);
    void btnToggle(int);
};

#endif // UPDATE698_H
