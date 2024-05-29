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
    setWindowTitle("Шифрование");
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


int f(int l, int n)
{
    return (l+n)%256;
}


QByteArray padding(QByteArray data)
{
    data += "1";
    int k = 2 - (data.size() % 2);
    for (int i = 0; i < k; i++)
    {
        data += "0";
    }
    return data;
}


QByteArray encryptionFestl(QByteArray data, QProgressBar *bar)
{
    data = padding(data);
    int div;
    mData *val = CreateList(1);
    val->a = 0;
    int k = 1;

    if (data.size() < 100)
    {
        div = 100/data.size();
        k = div;
    }
    else
        div = data.size()/100;

    QByteArray encp_data;
    int n = 3;
    QByteArray R;
    QByteArray L;
    L.append(data,data.size()/2);
    for (int i = data.size()/2; i < data.size(); i++)
    {
            R.append(data[i]);
        if (i%div == 0)
        {
            val->a += (2*k);
            bar->setValue(val->a);
        }
    }
    val->a = 0;
    for (int i = 0; i < data.size()/2; i++)
    {
        for (int j = 1; j < n; j++)
        {
            int temp = L[i];
            L[i] = R[i]^f(L[i],j);
            R[i] = temp;
        }
        R[i]^=f(L[i],n);

        if (i % div == 0 )
        {
            val->a += (2*k);
            bar->setValue(val->a);
        }

    }
    encp_data = L + R;
    DeleteList(val);
    return encp_data;
}


QByteArray hashing(QString pass)
{
    QString str_hash = QString::fromStdString(sha512(pass.toStdString()));
    QByteArray password = str_hash.toLocal8Bit();
    return password;
}


QByteArray encryptionXOR(QByteArray data, QByteArray pass, QProgressBar *bar)
{
    int h_len = 128;
    int div;
    mData *val = CreateList(1);
    val->a = 0;
    int k = 1;

    if (data.size() < 100)
    {
        div = 100/data.size();
        k = div;
    }
    else
        div = data.size()/100;

    QByteArray encp_data;

    for (int i = 0; i < data.size(); i++)
    {
        encp_data += data[i] ^ pass[i % h_len];
        if (data.size() >= 100)
        {
            if (i % div == 0 )
            {
                val->a += k;
                bar->setValue(val->a);
            }
        }
        else
        {
            val->a += k;
            bar->setValue(val->a);
        }
    }
    DeleteList(val);
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

    if ((ui->path_line->text() == "" || ui->pass_line->text() == "") && ui->comboBox->currentIndex() != 1)
    {
        ui->error->setText("Введены не все данные");
    }
    else if (ui->path_line->text() == "" && ui->comboBox->currentIndex() == 1)
    {
        ui->error->setText("Введены не все данные");
    }
    else
    {
        ui->error->setText("");

        QFile file(path);

        if (!file.open(QIODevice::ReadOnly))
        {
            ui->error->setText("Ошибка открытия файла");
        }
        else
        {
            // создание Progress Bar
            QProgressBar *bar = new QProgressBar(this);
            bar->setValue(0);
            ui->gridLayout->addWidget(bar,11,0,1,0);
            bar->show();

            if (file.size() == 0)
            {
                QFile encp_file(path+".encp");
                encp_file.open(QIODevice::WriteOnly);
                encp_file.close();
                file.close();
                bar->setValue(100);
                ui->error->setText("Файл сохранен в ту же папку");
            }
            else
            {
                // чтение пароля и данных из файла
                QByteArray pass = hashing(ui->pass_line->text());
                QByteArray data;
                long long s = file.size();
                mData *val = CreateList(1);
                val->a = 0;
                for (int i = file.size(); i > 0; i+=65536)
                {
                    if (s >= 65536)
                        data.append(file.read(65536));
                    else
                        data.append(file.readAll());
                    s-=65536;
                    bar->setValue((val->a)++);
                }
                QByteArray encp_data;

                // выбор метода шифрования
                // 0 - XOR, 1 - Фейстель
                if (ui->comboBox->currentIndex() == 0)
                    encp_data = encryptionXOR(data,pass,bar);
                else if (ui->comboBox->currentIndex() == 1)
                    encp_data = encryptionFestl(data,bar);


                QFile encp_file(path+".encp");
                if (!encp_file.open(QIODevice::WriteOnly))
                    ui->error->setText("Ошибка при создании файла");
                else
                {
                    // запись хэша
                    if (ui->comboBox->currentIndex() == 0)
                    {
                        if (ui->comboBox->currentIndex() == 0)
                            encp_file.write(pass);
                    }
                    // запись основных данных
                    encp_file.write(encp_data);

                    ui->error->setText("Файл сохранен в ту же папку");
                    //проверка

                }
                encp_file.close();
                DeleteList(val);
            }
            delete bar;

        }
    file.close();



    }
}

void Encrypt::on_comboBox_currentIndexChanged(int index)
{
    if (ui->comboBox->currentIndex() == 1)
        ui->pass_line->setDisabled(true);
    else
        ui->pass_line->setDisabled(false);
}

