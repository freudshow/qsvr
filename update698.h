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
    QByteArray m_fileblock;
    qint64 m_filesize;
    qint64 m_blockCount;

private slots:
    void browse();
    void on_btn_exit_clicked(bool);
};

#endif // UPDATE698_H
