#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(ButtonSlot()));
}

void MainWindow::ButtonSlot()
{
    path = "C:/qt proj/untitled/image.jpg";
    img_object = new QImage();
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    img_object->load(path);
    image = QPixmap::fromImage(*img_object);
    QPixmap scaled_img = image.scaled(ui->graphicsView->geometry().width(), ui->graphicsView->geometry().height(), Qt::KeepAspectRatio);
    scene->addPixmap(scaled_img);
    scene->setSceneRect(scaled_img.rect());


    //рисуем точку
    QPen Pen_1 (Qt::red);//контур
    QBrush Brush_1 (Qt::gray);//заполнение
    Pen_1.setWidth(3);//толщина контура
    int d=20;//диаметр точки
    int x = scaled_img.width() / 2 ,y = scaled_img.height() /2;//координаты желаемого центра точки







    //ui->label->setText(QString::number(x) + " " + QString::number(y) + "\n"
    //                   + QString::number(ui->graphicsView->geometry().width()) + " " + QString::number(ui->graphicsView->geometry().height()));
    scene->addEllipse(x - d/2,y - d/2,d,d,Pen_1,Brush_1);//смещаем координаты так чтобы центр оказался в нужном месте
    //т.к. круг рисуется внутри квадрата где x y - координаты левого верхнего угла.

    ui->graphicsView->setStyleSheet("background-color:black;");


    emit SignalFromButton();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    if (path != "")
    {
        scene = new QGraphicsScene(this);
        ui->graphicsView->setScene(scene);
        img_object = new QImage();
        img_object->load(path);
        image = QPixmap::fromImage(*img_object);
        QPixmap scaled_img = image.scaled(ui->graphicsView->geometry().width(), ui->graphicsView->geometry().height(), Qt::KeepAspectRatio);

        scene->addPixmap(scaled_img);
        scene->setSceneRect(scaled_img.rect());


    //    ui->graphicsView->setStyleSheet("background-color:black;");

    }

}

MainWindow::~MainWindow()
{
    delete ui;
}



