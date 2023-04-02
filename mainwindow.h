#ifndef MAINWINDOW\_H
#define MAINWINDOW\_H

#include <QMainWindow>
#include <QGraphicsSvgItem>
#include <QString>


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
    void DrawPoint();

private:
    QString path = "";
    Ui::MainWindow  *ui;
    QGraphicsScene *scene;
    QImage *img_object;
    QPixmap image;
    QGraphicsPixmapItem * pixmap_item;
    QPixmap scaled_img;
    QPoint Point;

signals:
    void SignalFromButton();
private slots:
    void ButtonSlot();
    void ButtonSlot2();
};

#endif // MAINWINDOW\_H
