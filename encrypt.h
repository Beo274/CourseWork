#ifndef ENCRYPT_H
#define ENCRYPT_H

#include <QDialog>

namespace Ui {
class Encrypt;
}

class Encrypt : public QDialog
{
    Q_OBJECT

public:
    explicit Encrypt(QWidget *parent = nullptr);
    ~Encrypt();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_comboBox_currentIndexChanged(int index);

private:
    Ui::Encrypt *ui;
};

#endif // ENCRYPT_H
