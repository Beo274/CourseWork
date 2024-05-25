#include "decrypt.h"
#include "ui_decrypt.h"
#include <string>
#include "sha512.h"
#include <QFileDialog>
#include <QFile>
#include <QString>
#include <QDebug>
#include <QDataStream>
#include "mainwindow.h"
#include <QProgressBar>
#include <QThread>
struct mData;
mData *CreateList(unsigned length);
void DeleteList(mData *&beg);
mData *getItem(mData *beg, unsigned index);
mData *addItem(mData *beg, unsigned index);
unsigned getLength(mData *beg);
void arrToList(mData *beg, char *arr);

using namespace std;


Decrypt::Decrypt(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Decrypt)
{
    ui->setupUi(this);
}

Decrypt::~Decrypt()
{
    delete ui;
}

QString encp_path = "";
char hash_entered[129];


QByteArray dcpHashing(QString pass)
{
    QString str_hash = QString::fromStdString(sha512(pass.toStdString()));
    QByteArray password = str_hash.toLocal8Bit();
    return password;
}


QByteArray decryptionXOR(QByteArray encp_data, QByteArray encp_hash, QProgressBar *bar)
{
    int div = encp_data.size()/100;
    int val = 0;
    uint h_len = 128;
    QByteArray dcp_data;
    for (int i = 0; i < encp_data.size(); i++)
    {
        dcp_data += (encp_data[i] ^ encp_hash[i % h_len]);
        if (i % div == 0)
        {
            QThread::msleep(1);
            bar->setValue(++val);
        }

    }
    return dcp_data;
}


QString editPath(QString encp_path)
{
    int index = encp_path.lastIndexOf(QChar('.'));
    QString or_path = encp_path.left(index);
    int or_index = or_path.lastIndexOf(QChar('.'));
    encp_path.insert(or_index,"_dcp");
    return encp_path.left(4+index);
}


void Decrypt::on_pushButton_clicked()
{
    encp_path = QFileDialog::getOpenFileName(this,QString("Open File"),QDir::currentPath(),"Encrypted files (*.encp)");
    if (!encp_path.isEmpty())
        ui->path->setText(encp_path);
}


void Decrypt::on_pushButton_2_clicked()
{
    if (ui->pass->text() == "" || ui->path->text() == "")
    {
        ui->error->setText("Введены не все данные");
    }
    else
    {
        ui->error->setText("");


        QFile encp_file(encp_path);
        if (!encp_file.open(QIODevice::ReadOnly))
        {
            ui->error->setText("Ошибка открытия файла");
        }
        else
        {
            // инициализация длин
            uint h_len = 128;
            uint d_len = encp_file.size() - h_len;

            // запись хэша и данных из файла
            QByteArray encp_hash = encp_file.read(h_len);
            QByteArray encp_data = encp_file.read(d_len);

            // хэширование введенного пароля и проверка на совпадение
            QByteArray pass = dcpHashing(ui->pass->text());
            qDebug() << ui->pass->text();
            bool isEqual = 1;
            for (uint i = 0; i < h_len; i++)
            {
                if (encp_hash[i] != pass[i])
                {
                    isEqual = 0;
                    break;
                }
            }
            if (!isEqual)
            {
                ui->error->setText("Неверный пароль");
            }
            else
            {
                ui->error->setText("");

                // создание ProgressBar
                QProgressBar *bar = new QProgressBar(this);
                bar->setValue(0);
                ui->gridLayout->addWidget(bar,10,0,1,2);
                bar->show();

                // дешифровка
                QByteArray dcp_data = decryptionXOR(encp_data,encp_hash, bar);

                //запись в файл
                QFile dcp_file(editPath(encp_path));
                if (!dcp_file.open(QIODevice::WriteOnly))
                {
                    ui->error->setText("Ошибка при создании файла");
                }
                else
                {
                    dcp_file.write(dcp_data);
                    ui->error->setText("Файл сохранен в ту же папку");
                }
                dcp_file.close();
                delete bar;
            }
        }
        encp_file.close();
    }
}


