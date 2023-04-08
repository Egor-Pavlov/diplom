#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include "device.h"
#include "datagenerator.h"
#include <QMessageBox>

//MainWindow::Generator()
//{

//}


void MainWindow::GetData()
{
    //типо запрос к бд

    //берем устройства у которых есть метка отображать
    //запрашиваем для всех кроме тех, у кого флаг отображения отрицательный точки свежее текущей
    //если есть флаг маршрута все кроме самой свежей пишем в маршрут, самую свежую принимаем за текущую координату
    //если устройство новое - создаем объект device

    DataGenerator generator = DataGenerator();
    QDateTime ReqTime = QDateTime::currentDateTime();
    QVector<std::pair<QString, Coordinate>> Data = generator.GenerateCoordinate(Devices, QPoint(scaled_img.width()/2, scaled_img.height()/2));

    while(Data.size() > 0)
    {
        int index = -1;
        for(int j = 0; j < Devices.size(); j++)
        {
            //проверяем что устройство с таким маком уже известно
            if(Data[0].first == Devices[j].GetMacAddres())
            {
                index = j;
                break;
            }
        }
        if(index >= 0 )//если известно, то обновляем координаты
        {
            Devices[index].UpdateCoord(Data[0].second);
            Data.removeFirst();
        }
        else
        {//если не известно запрашиваем имя и создаем объект
            Device * d = new Device(Data[0].first, generator.GenerateName(), Data[0].second, 20);
            Devices.append(*d);
            Data.removeFirst();
        }
    }
    //удаляем те по которым нет данных
    foreach (Device d, Devices)
    {
        if(d.GetCurrentCoord().GetDateTime() < ReqTime)
        Devices.removeAt(Devices.indexOf(d));
    }
//    //запрашиваем все точки после этого времени
//    //QString jsonData = Generator(time);
//    //парсим json
//    //передаем новые координаты в объекты или создаем новые объекты, удаляем объекты данных о которых не поступило
}


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->pushButton->setText("Нарисовать точку");
    connect(ui->displayAllDeviceButton, SIGNAL(clicked()), this, SLOT(DisplayAllSlot()));
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(ButtonSlot()));
    connect(ui->FileOpen, SIGNAL(triggered()), this, SLOT(FileOpenSlot()));

    QStringList headers;
    headers.append("Цвет");
    headers.append("Имя");
    headers.append("Отображать\nна карте");
    headers.append("Показать\nмаршрут");

    ui->tableWidget->setColumnCount(4); // Указываем число колонок
    ui->tableWidget->setShowGrid(true); // Включаем сетку
    // Разрешаем выделение только одного элемента
    ui->tableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    // Разрешаем выделение построчно
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    // Устанавливаем заголовки колонок
    ui->tableWidget->setHorizontalHeaderLabels(headers);
    // Растягиваем последнюю колонку на всё доступное пространство
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget->setColumnWidth(0, 40);
}

void MainWindow::ButtonSlot()
{
    if(path == "")
    {
        QMessageBox::warning(this, "Ошибка","Сначала нужно открыть файл!");
        return;
    }

    if(Devices.size() != 0)
    {
        //обновляем флаги отображения в объектах
        for(int i = 0; i < Devices.size(); i++)
        {
            Devices[i].SetDeviceVisible(ui->tableWidget->item(i, 2)->checkState());
            Devices[i].SetRouteVisible(ui->tableWidget->item(i, 3)->checkState());
        }
    }

    GetData();
    //рисуем точки и линии
    DrawScene();//очищаем сцену
    for (int i = 0; i < Devices.size(); i++)
    {
        DrawSingleDevice(Devices[i]);
    }
    UpdateList();
    emit SignalFromButton();
}

void MainWindow::FileOpenSlot()
{
    path = "C:/qt proj/untitled/image.jpg";
    //path = QFileDialog::getOpenFileName(this, "Выбор плана", "/", "*.jpg");
    DrawScene();
    //emit SignalFromButton();
}
void MainWindow::DrawScene()
{
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    QImage *img_object = new QImage();
    //QGraphicsPixmapItem * pixmap_item;
    img_object->load(path);
    QPixmap image = QPixmap::fromImage(*img_object);
    scaled_img = image.scaled(ui->graphicsView->geometry().width(), ui->graphicsView->geometry().height(), Qt::KeepAspectRatio);
    scene->addPixmap(scaled_img);
    scene->setSceneRect(scaled_img.rect());
}

void MainWindow::UpdateList()
{
    while(ui->tableWidget->rowCount() != 0)
    {
        ui->tableWidget->removeRow(0);
    }

    for(int i = 0; i < Devices.size(); i++)
    {
        ui->tableWidget->insertRow(ui->tableWidget->rowCount());
        //цвет
        ui->tableWidget->setItem(i,0, new QTableWidgetItem());
        ui->tableWidget->item(i,0)->setBackground(QBrush(Devices[i].GetColor()));
        //имя
        ui->tableWidget->setItem(i,1, new QTableWidgetItem(Devices[i].GetName()));
        //отображать?
        QTableWidgetItem *item = new QTableWidgetItem();
        if(Devices[i].GetDeviceVisible())
            item->setCheckState(Qt::Checked);
        else
            item->setCheckState(Qt::Unchecked);

        item->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(i, 2, item);
        //показать маршрут?
        item = new QTableWidgetItem();

        if(Devices[i].GetRouteVisible())
            item->setCheckState(Qt::Checked);
        else
            item->setCheckState(Qt::Unchecked);

        item->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(i, 3, item);
    }
}

void MainWindow::DrawSingleDevice(Device d)
{
    if(d.GetDeviceVisible())
    {
        QPen Pen_1 (d.GetColor());//контур
        QBrush Brush_1 (d.GetColor());//заполнение

        Pen_1.setWidth(3);//толщина контура
        int D=20;//диаметр точки

        //рисуем маршрут
        if (d.GetRouteVisible())
        {
            QVector <Coordinate> points = d.GetRoute();
            points.push_front(d.GetCurrentCoord());

            for(int i = 0; i < points.size() - 1; i++)
            {
                scene->addLine(points[i].GetX(), points[i].GetY(), points[i + 1].GetX(), points[i + 1].GetY(), Pen_1);
            }
        }
        //смещаем координаты так чтобы центр оказался в нужном месте
        //т.к. круг рисуется внутри квадрата где x y - координаты левого верхнего угла.
        scene->addEllipse(d.GetCurrentCoord().GetX() - D/2, d.GetCurrentCoord().GetY() - D/2,D,D,Pen_1,Brush_1);
    }
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

        if(Devices.size() > 0)
        {
            for (int i = 0; i < Devices.size(); i++)
            {
                Devices[i].RecalcCoord(Coef);
                DrawSingleDevice(Devices[i]);
            }
        }
    }
}

void MainWindow::DisplayAllSlot()
{
    //ставим галочки во все строки таблицы
    for(int i = 0; i < ui->tableWidget->rowCount(); i++)
    {
        ui->tableWidget->item(i, 2)->setCheckState(Qt::Checked);
    }
    emit SignalFromButton();
}

MainWindow::~MainWindow()
{
    delete ui;
}



