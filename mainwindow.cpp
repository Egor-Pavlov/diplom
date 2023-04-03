#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "device.cpp"


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->pushButton->setText("Нарисовать точку");
    ui->pushButton_2->setText("Открыть картинку");
    Point = QPoint(0, 0);

    Device * d = new Device(QString::number(0 + rand() % 20), "mobile",
                            QPoint(0, 0), 100);
    Devices.append(*d);

    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(ButtonSlot()));
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(ButtonSlot2()));
}

void MainWindow::ButtonSlot()
{
    //спавн в центре потому что генерируется


    //ПОТОМ НАДО УБРАТЬ!!!


    //==========================================================================================================
    if (Devices[0].GetCurrentCoord().x() <= 0 || Devices[0].GetCurrentCoord().x() >= scaled_img.width()
            || Devices[0].GetCurrentCoord().y() <= 0 || Devices[0].GetCurrentCoord().y() >= scaled_img.height())
    {
        Devices[0].UpdateCoord(QPoint(scaled_img.width() / 2, scaled_img.height() / 2), false);
    }

    //==========================================================================================================
    else
    {
        Devices[0].UpdateCoord(QPoint(Devices[0].GetCurrentCoord().x() + (-30 + rand() % 60),
                           Devices[0].GetCurrentCoord().y() + (-30 + rand() % 60)), ui->RouteCB->checkState());
    }

    //рисуем точку
    DrawSingleDevice(Devices[0]);
    emit SignalFromButton();
}

void MainWindow::ButtonSlot2()
{
    path = "C:/qt proj/untitled/image.jpg";

    DrawScene();
    emit SignalFromButton();
}
void MainWindow::DrawScene()
{
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    img_object = new QImage();
    img_object->load(path);
    image = QPixmap::fromImage(*img_object);
    scaled_img = image.scaled(ui->graphicsView->geometry().width(), ui->graphicsView->geometry().height(), Qt::KeepAspectRatio);

    scene->addPixmap(scaled_img);
    scene->setSceneRect(scaled_img.rect());
}

void MainWindow::DrawSingleDevice(Device d)
{
    DrawScene();

    //потом это будет генерироваться по мак адресу и возможно лежать в объекте
    QPen Pen_1 (Qt::red);//контур
    QBrush Brush_1 (Qt::gray);//заполнение
    //====================================

    Pen_1.setWidth(3);//толщина контура
    int D=20;//диаметр точки

    //рисуем маршрут
    if (ui->RouteCB->checkState())
    {
        QVector <QPoint> points = d.GetRoute();
        points.push_front(d.GetCurrentCoord());

        for(int i = 0; i < points.size() - 1; i++)
        {
            scene->addLine(points[i].x(), points[i].y(), points[i + 1].x(), points[i + 1].y(), Pen_1);
        }

    }
    //смещаем координаты так чтобы центр оказался в нужном месте
    //т.к. круг рисуется внутри квадрата где x y - координаты левого верхнего угла.
    scene->addEllipse(d.GetCurrentCoord().x() - D/2, d.GetCurrentCoord().y() - D/2,D,D,Pen_1,Brush_1);
}
void MainWindow::resizeEvent(QResizeEvent *event)
{
    if (path != "")
    {
        //запоминаем размеры до увеличения
        int w = scaled_img.width();
        int h = scaled_img.height();
        //коэффициент во сколько раз размер окна больше изначального
        double Coef;

        //подгоняем картинку под новые размеры
        DrawScene();

        Coef = ((double) scaled_img.width()) /((double) w);


        for (int i = 0; i < Devices.size(); i++)
        {
            Devices[i].RecalcCoord(Coef);
            DrawSingleDevice(Devices[i]);
        }

    }
}


MainWindow::~MainWindow()
{
    delete ui;
}



