#ifndef TOOLSDIALOG_H
#define TOOLSDIALOG_H

#include <QDialog>
#include <QRadioButton>
#include <QMessageBox>
#include <QMap>
#include "libutil/basedef.h"

typedef enum calcMethod {
    e_calcNothing = -1,
    e_calcCRC16 = 0,
    e_calcFCS,
    e_calcSumChk,
    e_calcAdd33,
    e_calcMinus33,
    e_calcInverse
} calcMethod_e;



namespace Ui {
class toolsDialog;
}

class toolsDialog;
typedef void (toolsDialog::*calc_f)();

class toolsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit toolsDialog(QWidget *parent = nullptr);
    ~toolsDialog();

private:
    Ui::toolsDialog *ui;

    calcMethod_e m_calcMethod;
    QMap<calcMethod_e, calc_f> m_map;

    void calcCRC16();
    void calcFCS();
    void calcSumChk();
    void calcAdd33();
    void calcMinus33();
    void calcInverse();

private slots:
    void on_btn_calc_clicked();
    void on_btn_exit_clicked();
    void on_btn_copy_clicked();

    void btnToggle(int);
    void prepareText(); //检查textEdit的输入字符串格式是否合法, 并将textEdit的字符串自动用空格分隔

signals:
    void toolExit();
};

#endif // TOOLSDIALOG_H
