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
int f(int l, int n);

using namespace std;


Decrypt::Decrypt(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Decrypt)
{
    ui->setupUi(this);
    setWindowTitle("Дешифрование");
}

Decrypt::~Decrypt()
{
    delete ui;
}

QString encp_path = "";
char hash_entered[129];

QByteArray unpadding(QByteArray data)
{
    for (int i = data.size()-1; i >= 0; i--)
    {
        if (data[i] == '1')
        {
            data.removeAt(i);
            return data;
        }
        data.removeAt(i);
    }
}

QByteArray decryptionFestl(QByteArray encp_data, QProgressBar *bar)
{

    int div;
    int val = 0;
    int k = 1;

    if (encp_data.size() < 100)
    {
        div = 100/encp_data.size();
        k = div;
    }
    else
        div = encp_data.size()/100;
    int n = 3;
    QByteArray R;
    QByteArray L;

    for (uint i = 0; i < encp_data.size(); i++)
    {
        if (i < encp_data.size()/2)
            L+=encp_data[i];
        else
            R+=encp_data[i];
    }

    for (int i = 0; i < encp_data.size()/2; i++)
    {
        for (int j = n; j > 1; j--)
        {
            int temp = L[i];
            L[i] = R[i]^f(L[i],j);
            R[i] = temp;
        }
        R[i]^=f(L[i],1);

        if (encp_data.size() >= 100)
        {
            if (i % div == 0 )
            {
                val+=(2*k);
                bar->setValue(val);
            }
        }
        else
        {
            val+=2*k;
            bar->setValue(val);
        }
    }
    bar->setValue(0);
    QByteArray dcp_data = L + R;
    bar->setValue(100);
    dcp_data = unpadding(dcp_data);
    return dcp_data;
}

QByteArray dcpHashing(QString pass)
{
    QString str_hash = QString::fromStdString(sha512(pass.toStdString()));
    QByteArray password = str_hash.toLocal8Bit();
    return password;
}


QByteArray decryptionXOR(QByteArray encp_data, QByteArray encp_hash, QProgressBar *bar)
{
    int div;
    int val = 0;
    int k = 1;

    if (encp_data.size() < 100)
    {
        div = 100/encp_data.size();
        k = div;
    }
    else
        div = encp_data.size()/100;

    uint h_len = 128;
    QByteArray dcp_data;
    for (int i = 0; i < encp_data.size(); i++)
    {
        dcp_data += (encp_data[i] ^ encp_hash[i % h_len]);
        if (encp_data.size() >= 100)
        {
            if (i % div == 0 )
            {
                val+=k;
                bar->setValue(val);
            }
        }
        else
        {
            val+=k;
            bar->setValue(val);
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
    if ((ui->path->text() == "" || ui->pass->text() == "") && ui->comboBox->currentIndex() == 0)
    {
        ui->error->setText("Введены не все данные");
    }
    else if (ui->path->text() == "" && ui->comboBox->currentIndex() == 2)
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
            // создание ProgressBar
            QProgressBar *bar = new QProgressBar(this);
            bar->setValue(0);
            ui->gridLayout->addWidget(bar,10,0,1,2);
            bar->show();

            if (encp_file.size() == 0)
            {
                QFile dcp_file(editPath(encp_path));
                dcp_file.open(QIODevice::WriteOnly);
                dcp_file.close();
                encp_file.close();
                bar->setValue(100);
                ui->error->setText("Файл сохранен в ту же папку");
            }
            else
            {
                // инициализация длин
                uint h_len = 128;
                uint d_len = encp_file.size() - h_len;

                // инициализация расшифрованных массивов байт
                QByteArray dcp_data;

                // хэширование введенного пароля и проверка на совпадение
                if (ui->comboBox->currentIndex() == 0)
                {
                    // запись хэша и данных из файла
                    QByteArray encp_hash = encp_file.read(h_len);
                    QByteArray encp_data = encp_file.read(d_len);

                    QByteArray pass = dcpHashing(ui->pass->text());
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
                        // дешифровка
                        dcp_data = decryptionXOR(encp_data,encp_hash, bar);
                    }
                }
                else if (ui->comboBox->currentIndex() == 1)
                {
                    QByteArray encp_data = encp_file.readAll();
                    dcp_data = decryptionFestl(encp_data,bar);
                }

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
            }
            delete bar;
        }
        encp_file.close();
    }
}



void Decrypt::on_comboBox_currentIndexChanged(int index)
{
    if (ui->comboBox->currentIndex() == 1)
    {
        ui->pass->setDisabled(true);
    }
    else
    {
        ui->pass->setDisabled(false);
    }
}

