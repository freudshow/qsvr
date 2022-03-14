#ifndef MISCTOOLS_H
#define MISCTOOLS_H

#include <QMainWindow>

namespace Ui {
class miscTools;
}

class miscTools : public QMainWindow
{
    Q_OBJECT

public:
    explicit miscTools(QWidget *parent = nullptr);
    ~miscTools();

private slots:
    void on_miscTools_iconSizeChanged(const QSize &iconSize);

private:
    Ui::miscTools *ui;
};

#endif // MISCTOOLS_H
