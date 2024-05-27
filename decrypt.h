#ifndef DECRYPT_H
#define DECRYPT_H

#include <QDialog>

namespace Ui {
class Decrypt;
}

class Decrypt : public QDialog
{
    Q_OBJECT

public:
    explicit Decrypt(QWidget *parent = nullptr);
    ~Decrypt();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_comboBox_currentIndexChanged(int index);

private:
    Ui::Decrypt *ui;
};

#endif // DECRYPT_H
