#include "encrypt.h"
#include "ui_encrypt.h"
#include "mainwindow.h"
#include <string>
#include "sha512.h"
#include <QFileDialog>
#include <QFile>
#include <QString>
#include <QDebug>
#include <QProgressBar>
#include <cmath>
#include <QThread>
struct mData;
using namespace std;

Encrypt::Encrypt(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Encrypt)
{
    ui->setupUi(this);
}


Encrypt::~Encrypt()
{
    delete ui;
}

QString path = "";
char char_hash[129];

mData *CreateList(unsigned length)
{
    mData *curr = 0, *next = 0;
    for (unsigned i = 1; i <= length; i++)
    {
        curr = new mData;
        curr->adr = next;
        next = curr;
    }
    return curr;
}


void DeleteList(mData *&beg)
{
    mData *next;
    while(beg)
    {
        next  = beg -> adr;
        delete beg;
        beg = next;
    }
}


mData *getItem(mData *beg, unsigned index)
{
    while (beg && (index--))
        beg = beg->adr;
    return beg;
}


mData *addItem(mData *beg, unsigned index)
{
    mData *item = new mData;
    if (!beg || !index)
    {
        item->adr = beg;
        beg = item;
        return item;
    }
    mData *predItem = beg;
    --index;
    while(predItem->adr && (index--))
        predItem = predItem->adr;
    item->adr = predItem->adr;
    predItem->adr = item;
    return item;
}


unsigned getLength(mData *beg)
{
    unsigned length = 0;
    while (beg)
    {
        ++length;
        beg = beg->adr;
    }
    return length;
}

// int divs(int size)
// {
//     int max = 2;
//     for (int div = 1; div < int(std::pow(size,0.5))+1; div++)
//     {
//         if (size % div == 0 && div != 1)
//         {
//             if (div*100/size != 0)
//             {
//                 return div*100/size;
//             }
//             continue;
//         }
//     }
//     return 1;
// }

QByteArray hashing(QString pass)
{
    QString str_hash = QString::fromStdString(sha512(pass.toStdString()));
    QByteArray password = str_hash.toLocal8Bit();
    return password;
}


mData *encryptionXOR(QByteArray data, QByteArray pass, QProgressBar *bar)
{
    int h_len = 128;
    int div = data.size()/100;
    int val=0;
    mData *encp_data = CreateList(data.size());
    for (int i = 0; i < data.size(); i++)
    {
        getItem(encp_data, i)->a = data[i] ^ pass[i % h_len];
        if (i % div == 0 )
        {
            QThread::msleep(1);
            bar->setValue(++val);
        }

    }
    return encp_data;
}


void Encrypt::on_pushButton_clicked()
{
    path = QFileDialog::getOpenFileName(this,QString("Open File"),QDir::currentPath(),"All files (*.*)");
    if (!path.isEmpty())
        ui->path_line->setText(path);
}


void Encrypt::on_pushButton_2_clicked()
{
    //Обнуление переменных
    ui->error->setText("");
    const char *nothing = {""};
    for (unsigned i = 0; i <= 128; i++)
        char_hash[i] = nothing[0];

    if (ui->path_line->text() == "" || ui->pass_line->text() == "")
    {
        ui->error->setText("Введены не все данные");
    }
    else
    {
        ui->error->setText("");

        QFile file(path);
        uint len = file.size();

        if (!file.open(QIODevice::ReadOnly))
        {
            ui->error->setText("Ошибка открытия файла");
        }
        else
        {
            QProgressBar *bar = new QProgressBar(this);
            bar->setValue(0);
            ui->gridLayout->addWidget(bar,10,0,1,0);
            bar->show();

            QByteArray pass = hashing(ui->pass_line->text());
            qDebug() << ui->pass_line->text();
            QByteArray data = file.readAll();
            mData *encp_data = encryptionXOR(data,pass,bar);
            QByteArray encp_arr;

            for (uint i = 0; i < len; i++)
                encp_arr += getItem(encp_data, i)->a;

            QFile encp_file(path+".encp");
            if (!encp_file.open(QIODevice::WriteOnly))
                ui->error->setText("Ошибка при создании файла");
            else
            {
                ui->error->setText("");

                // запись хэша
                encp_file.write(pass);

                // запись основных данных
                encp_file.write(encp_arr);

                ui->error->setText("Файл сохранен в ту же папку");
            }
            DeleteList(encp_data);
            encp_file.close();
            delete bar;
        }
    file.close();



    }
}
