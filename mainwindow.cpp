 #include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    encrypt = new Encrypt(this);
    decrypt = new Decrypt(this);
    setWindowTitle("Главная страница");
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


void MainWindow::on_help_button_clicked()
{
    QMessageBox::information(this,"Помощь","Для шифирования файла, выберите соответствующий пункт меню,\nвыбирите нужный вам файл, введите пароль шифрования и нажмите кнопку ЗАШИФРОВАТЬ\nДля дешифровки выполните аналогичные действия, вписав в соответсвующую графу пароль, которым вы шифровали файл");
}

