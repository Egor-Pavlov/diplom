#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

#include <QTcpServer>
#include <QTcpSocket>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog();
    void closeEvent(QCloseEvent *event) override;
    void setParams(const QString &ip, const int &port);
signals:
    // Сигнал, который будет передавать данные другому окну
    void dataReady(const QString& data);

private slots:
    // Слот, который вызывается при нажатии кнопки
    void sendData();
    void on_okButton_clicked();

private:

    QString IP = "127.0.0.1";
    int port = 8080;

    Ui::Dialog *ui;
    QByteArray Data;
};

#endif // DIALOG_H
