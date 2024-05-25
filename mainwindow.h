#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "encrypt.h"
#include "decrypt.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE


struct mData
{
    char a;
    mData *adr;
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();



private slots:
    void on_pushButton_clicked();

    void on_decrypt_button_clicked();

    void on_help_button_clicked();

private:
    Ui::MainWindow *ui;
    Encrypt *encrypt;
    Decrypt *decrypt;
};
#endif // MAINWINDOW_H
