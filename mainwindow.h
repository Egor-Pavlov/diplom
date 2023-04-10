#ifndef MAINWINDOW\_H
#define MAINWINDOW\_H
#include <QMainWindow>
#include <QGraphicsSvgItem>
#include <QString>
#include "device.h"
#include <QVector>
#include <QTableWidget>
#include <QTimer>

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
    Ui::MainWindow  *ui;
    QGraphicsScene *scene;
    QString path = "";
    QPixmap scaled_img;
    QVector <Device> Devices;
    QTimer * Timer;

    void GetData();
    void UpdateList();
    QString Generator(QTime time);


signals:
    void SignalFromButton();

private slots:
    void FileOpenSlot();
    void DisplayAllSlot();
    void ButtonSlot();
    void StartStopSlot();

    void slotTimerAlarm();

};

#endif // MAINWINDOW\_H
