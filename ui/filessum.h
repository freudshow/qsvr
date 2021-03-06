#ifndef FILESSUM_H
#define FILESSUM_H

#include <QWidget>
#include <QRadioButton>
#include <QMessageBox>
#include <QMap>
#include "libutil/basedef.h"

typedef enum sumFileMethod {
    e_sumFileNothing = -1,
    e_sumFileCRC16 = 0,
    e_sumFileMd5,
    e_sumFileSha1,
    e_sumFileSha256,
} sumFileMethod_e;

namespace Ui {
class filesSum;
}

class filesSum;
typedef void (filesSum::*sumFIle_f)();

class filesSum : public QWidget
{
    Q_OBJECT

public:
    explicit filesSum(QWidget *parent = nullptr);
    ~filesSum();

private slots:
    void calcSum();

    void sumFileCrc16();
    void sumFileMd5();
    void sumFileSha1();
    void sumFileSha256();

    void on_btn_choose_clicked();
    void on_btn_exit_clicked();
    void on_btn_calc_clicked();

    void btnToggle(int idx);

    void showEvent(QShowEvent * event);
    void closeEvent(QCloseEvent * event);
    void resizeEvent(QResizeEvent * event);
    u16 crcfile(QString filename);
private:
    Ui::filesSum *ui;

    sumFileMethod_e m_sumFileMethod;
    QMap<sumFileMethod_e, sumFIle_f> m_map;

signals:
    void fileClose();
};

#endif // FILESSUM_H
