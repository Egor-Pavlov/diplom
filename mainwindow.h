#ifndef MAINWINDOW\_H
#define MAINWINDOW\_H
#include <QMainWindow>
#include <QGraphicsSvgItem>
#include <QString>
#include "device.h"
#include <QVector>

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
    void DrawScene();
    void DrawSingleDevice(Device d);

private:
    QString path = "";
    Ui::MainWindow  *ui;
    QGraphicsScene *scene;
    QImage *img_object;
    QPixmap image;
    QGraphicsPixmapItem * pixmap_item;
    QPixmap scaled_img;
    QPoint Point;
    QVector <Device> Devices;

    void GetData();
    void UpdateList();
    QString Generator(QTime time);

signals:
    void SignalFromButton();
    void SignalFromCB();

private slots:
    void FileOpenSlot();
    void CBSlot();
    void ButtonSlot();

};

#endif // MAINWINDOW\_H
