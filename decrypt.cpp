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


void dcpHashing(QString pass)
{
    const int h_len = 128;
    string str_hash = sha512(pass.toStdString());
    for (int i = 0; i < h_len; i++)
    {
        hash_entered[i] = char(str_hash[i]);
    }
}


QByteArray decryptionXOR(QByteArray encp_data, QByteArray encp_hash)
{
    uint h_len = 128;
    QByteArray dcp_data;
    for (int i = 0; i < encp_data.size(); i++)
        dcp_data += (encp_data[i] ^ encp_hash[i % h_len]);
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
            uint h_len = 128;
            uint d_len = encp_file.size() - h_len;

            // запись хэша и данных из файла
            QByteArray encp_hash = encp_file.read(h_len);
            QByteArray encp_data = encp_file.read(d_len);

            // хэширование введенного пароля и проверка на совпадение
            dcpHashing(ui->pass->text());
            bool isEqual = 1;
            for (uint i = 0; i < h_len; i++)
            {
                if (encp_hash.data()[i] != hash_entered[i])
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

                // дешифровка
                QByteArray dcp_data = decryptionXOR(encp_data,encp_hash);

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
        }
        encp_file.close();
    }
}


