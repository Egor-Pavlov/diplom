#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->pushButton->setText("Нарисовать точку");
    ui->pushButton_2->setText("Открыть картинку");
    Point = QPoint(0, 0);


    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(ButtonSlot()));
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(ButtonSlot2()));
}

void MainWindow::ButtonSlot()
{

    //рисуем точку
    DrawPoint();

    //ui->graphicsView->setStyleSheet("background-color:black;");


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

void MainWindow::DrawPoint()
{
    DrawScene();
    QPen Pen_1 (Qt::red);//контур
    QBrush Brush_1 (Qt::gray);//заполнение
    Pen_1.setWidth(3);//толщина контура
    int d=20;//диаметр точки
    Point.setX( Point.x() + (-10 + rand() % 20));
    Point.setY( Point.y() + (-10 + rand() % 20));

    if((Point.x() <= 0) || (Point.y() <= 0) || (Point.x() >= scaled_img.width()) || (Point.y() >= scaled_img.height()))
    {
        Point.setX(scaled_img.width() / 2);
        Point.setY(scaled_img.height() / 2);
    }

    //int x = 0 + rand() % scaled_img.width(), y = 0 + rand() % scaled_img.height();//координаты желаемого центра точки

    scene->addEllipse(Point.x() - d/2, Point.y() - d/2,d,d,Pen_1,Brush_1);//смещаем координаты так чтобы центр оказался в нужном месте
    //т.к. круг рисуется внутри квадрата где x y - координаты левого верхнего угла.
}
void MainWindow::resizeEvent(QResizeEvent *event)
{
    if (path != "")
    {
       DrawScene();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}



