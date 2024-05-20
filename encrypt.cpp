#include "encrypt.h"
#include "ui_encrypt.h"
#include "mainwindow.h"
#include <string>
#include "sha512.h"
#include <QFileDialog>
#include <QFile>
#include <QString>
#include <QDebug>

// СДЕЛАТЬ ВСЕ ЧЕРЕЗ QByreArray
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


// void listToArr(mData *list, char *arr)
// {
//     uint len = getLength(list);
//     for (uint i = 0; i < len; i++)
//     {
//         arr[i] = getItem(list,i)->a;
//     }
// }


// void arrToList(mData *list, char *arr)
// {
//     uint len = getLength(list);
//     mData *curr = list;
//     for (int i = 0; i < len; i++)
//     {
//         curr->a = arr[i];
//         curr = curr->adr;
//     }
// }


QByteArray hashing(QString pass)
{
    const int h_len = 128;
    QString str_hash = QString::fromStdString(sha512(pass.toStdString()));
    QByteArray password = str_hash.toLocal8Bit();
    // for (int i = 0; i < h_len;i++)
    // {
    //     char_hash[i] = char(str_hash[i]);
    // }
    return password;
}


mData *encryptionXOR(QByteArray data, QByteArray pass)
{
    int h_len = 128;
    mData *encp_data = CreateList(data.size());
    //qDebug() << (typeid(pass[0 % h_len]) == typeid(char));

    for (int i = 0; i < data.size(); i++)
    {
        //qDebug() << i;
        getItem(encp_data, i)->a = data[i] ^ pass[i % h_len];
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
    QDebug debug = qDebug();
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
        uint h_len = 128;
        if (!file.open(QIODevice::ReadOnly))
        {
            ui->error->setText("Ошибка открытия файла");
        }
        else
        {
            QByteArray pass = hashing(ui->pass_line->text());

            QByteArray data = file.readAll();
            mData *encp_data = encryptionXOR(data,pass);
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

            // //тестирование
            // // QFile test(path+".encp");
            // // test.open(QIODevice::ReadOnly);
            // // uint a = 128;
            // // QByteArray th = ui->pass_line->text().toLocal8Bit();
            // // QByteArray td = test.read(test.size()-a);
            // // QByteArray res1;
            // // qDebug() << td.size() << "после записи";
            // // for (int i = 0; i <= td.size()-1; i++)
            // // {
            // //     res1[i] = td[i]^th[i%h_len];
            // // }
            // // qDebug() << "3";
            // // QFile test2(path+".txt");
            // // test2.open(QIODevice::WriteOnly);
            // // test2.write(res1);
            // // test2.close();
            // //test.close();

            // QFile f(path+".encp");
            // f.open(QIODevice::ReadOnly);
            // qDebug() << f.size() << "весь файл";
            // QByteArray fp = f.read(128);
            // QByteArray buf = f.read(f.size()-128);;
            // qDebug() << buf.size() << "после записи";
            // //QByteArray pass = ui->pass_line->text().toLocal8Bit();
            // for (int i = 0; i < buf.size(); i++)
            // {
            //     buf[i] = buf[i] ^ fp[i%128];
            // }
            // QFile f2(path+"1");
            // f2.open(QIODevice::WriteOnly);
            // f2.write(buf);
            // f.close();
            // f2.close();
            }
    file.close();

    }
}
