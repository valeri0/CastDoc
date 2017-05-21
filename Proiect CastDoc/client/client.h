#ifndef CLIENT_H
#define CLIENT_H

#include <QMainWindow>

namespace Ui {
class client;
}

class client : public QMainWindow
{
    Q_OBJECT

public:
    explicit client(QWidget *parent = 0);
    ~client();

private slots:
    void on_pushButton_clicked();

    void on_lineEdit_textChanged(const QString &arg1);

    void on_comboBox_currentIndexChanged(const QString &arg1);

    void on_pushButton_2_clicked();

    void on_pushButton_2_clicked(bool checked);

    void on_comboBox_currentTextChanged(const QString &arg1);

    void on_pushButton_3_clicked();

private:
    Ui::client *ui;
};

#endif // CLIENT_H
