#ifndef MAINWINDOW\_H
#define MAINWINDOW\_H
#include <QMainWindow>
#include <QGraphicsSvgItem>
#include <QString>
#include "device.h"
#include <QVector>
#include <QTableWidget>
#include <QTimer>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include <dialog.h>
#include <chooseareadialog.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    virtual void resizeEvent(QResizeEvent *event) override;
    void on_tableWidget_itemClicked(QTableWidgetItem * item);
    void DrawScene();
    void DrawSingleDevice(Device d);

private:
    //пользовательское
    QString Token = "";
    int areaId = -1;
    int buildingId = -1;

    QString IP = "127.0.0.1";
    int port = 8080;
    int interval = 900;//интервал промотки кнопками

    int PlanW = 0; //изначальная высота картинки

    //служебное
    Ui::MainWindow  *ui;
    QGraphicsScene *scene;
    QString path = "";
    QPixmap scaled_img;
    QVector <Device> Devices;
    QTimer * Timer;

    void UpdateList();
    QString Generator(QTime time);

    QByteArray Data;

    quint32 nextBlockSize = 0;//размер блока данных
    void sendToServer(QString str);
    void unpackData(const QByteArray& data);

    Dialog dialog;//окно авторизации
    ChooseAreaDialog AreaDialog;


    QMap<int, QString> RespCodes = {
            {401, "Вы не авторизованы"},
            {403, "Доступ запрещен"},
            {404, "не найдено"}
        };

signals:
    void SignalFromButton();

private slots:
    void FileOpenSlot();
    void AuthSlot();
    void DisplayAllSlot();
    void ButtonSlot();
    void StartStopSlot();

    void slotTimerAlarm();

    void onDataReceived(const QString& data);
    void onAreaChoosed(const QString& data);

    void on_showDevicesButton_clicked();
    void on_BackButt_clicked();
    void on_ForwardButt_clicked();
    void on_dateTimeEdit_dateTimeChanged(const QDateTime &dateTime);
};

#endif // MAINWINDOW\_H
