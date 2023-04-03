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


signals:
    void SignalFromButton();
private slots:
    void ButtonSlot();
    void ButtonSlot2();
};

#endif // MAINWINDOW\_H
