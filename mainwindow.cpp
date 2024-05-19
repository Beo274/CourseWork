#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    encrypt = new Encrypt(this);
    decrypt = new Decrypt(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_pushButton_clicked()
{
    encrypt->setModal(true);
    encrypt->exec();
}


void MainWindow::on_decrypt_button_clicked()
{
    decrypt->setModal(true);
    decrypt->exec();
}

