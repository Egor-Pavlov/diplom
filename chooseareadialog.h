#ifndef CHOOSEAREADIALOG_H
#define CHOOSEAREADIALOG_H

#include <QDialog>
#include <QNetworkAccessManager>
#include <Building.h>

namespace Ui {
class ChooseAreaDialog;
}

class ChooseAreaDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChooseAreaDialog(QWidget *parent = nullptr);
    ~ChooseAreaDialog();
    void closeEvent(QCloseEvent *event) override;
    void setParams(const QString &ip, const int &port, const QString &token);

signals:
    // Сигнал, который будет передавать данные другому окну
    void areaChoosed(const QString& data);

private slots:
    // Слот, который вызывается при нажатии кнопки
    void sendData();
    void onBuildingClicked();

    void on_OKButton_clicked();

private:
    QString IP = "127.0.0.1";
    int port = 8080;
    QByteArray Data;
    QString Token = "";
    Ui::ChooseAreaDialog *ui;

    QList<Building> Buildings;
    QList<QPair<int, QString>> Areas;

    // Создаем менеджер доступа к сети
    QNetworkAccessManager manager;

    void MakeRequest(const QUrl &url);
    void unpackData(const QByteArray& data);

    QMap<int, QString> RespCodes = {
            {401, "Вы не авторизованы"},
            {403, "Доступ запрещен"},
            {404, "не найдено"}
        };

};

#endif // CHOOSEAREADIALOG_H
