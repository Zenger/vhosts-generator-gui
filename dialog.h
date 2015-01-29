#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

private slots:
    void on_dialogCancel_clicked();

    void on_browseApache_clicked();

    void on_browseHosts_clicked();

    void on_dialogSave_clicked();

private:
    Ui::Dialog *ui;
};

#endif // DIALOG_H
