#include "device.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QFileDialog>
#include <QJsonArray>
#include <QDebug>
#include <QDir>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrlQuery>
#include <QEventLoop>

#define INTERVAL 60//интервал времени для выборки из бд

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->dateTimeEdit->setDateTime(QDateTime::currentDateTime());

    Timer = new QTimer();

    connect(Timer, SIGNAL(timeout()), this, SLOT(slotTimerAlarm()));
    connect(ui->displayAllDeviceButton, SIGNAL(clicked()), this, SLOT(DisplayAllSlot()));
    connect(ui->PlayButt, SIGNAL(clicked()), this, SLOT(StartStopSlot()));
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(ButtonSlot()));
    connect(ui->ChooseArea, SIGNAL(triggered()), this, SLOT(FileOpenSlot()));
    connect(ui->authButton, SIGNAL(triggered()), this, SLOT(AuthSlot()));

    connect(&dialog, &Dialog::dataReady, this, &MainWindow::onDataReceived);
    connect(&AreaDialog, &ChooseAreaDialog::areaChoosed, this, &MainWindow::onAreaChoosed);

    QStringList headers;
    headers.append("Цвет");
    headers.append("Имя");
    headers.append("Отображать\nна карте");
    headers.append("Показать\nмаршрут\n(длина)");
    //headers.append("Длина\nмаршрута");

    ui->tableWidget->setColumnCount(headers.size()); // Указываем число колонок
    ui->tableWidget->setShowGrid(true); // Включаем сетку
    // Разрешаем выделение только одного элемента
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    // Разрешаем выделение построчно
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    // Устанавливаем заголовки колонок
    ui->tableWidget->setHorizontalHeaderLabels(headers);
    // Растягиваем последнюю колонку на всё доступное пространство
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget->setColumnWidth(0, 40);
}

void MainWindow::unpackData(const QByteArray& data)
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
    QJsonObject jsonObj = jsonDoc.object();

    if(jsonObj["code"].toInt() != 200)
    {
        QMessageBox::warning(this, "Ошибка",RespCodes.value(jsonObj["code"].toInt()));
        Timer->stop();
        ui->PlayButt->setText("▶");

        if(jsonObj["code"].toInt() == 401)
        {
            Token = "";
            path = "";
            areaId = -1;
            UpdateList();//переписываем список
            DrawScene();//очищаем сцену
        }
        return;
    }

    QJsonArray jsonCoords = jsonObj["coords"].toArray();
    bool flag = true;
    QJsonObject jsonCoord;
    //qDebug() << jsonCoords;
    for (int i = 0; i < jsonCoords.size(); i++)
    {
        int w = scaled_img.width();//размер картинки на экране
        double coef = ((double) PlanW) /((double) w);//во сколько раз изначальный размер больше того который на экране

        jsonCoord = jsonCoords[i].toObject();
        flag = true;
        //так как координаты относительно изначального размера то делим их на коэффициент
        Coordinate c = Coordinate(jsonCoord["x"].toInt()/coef, jsonCoord["y"].toInt()/coef,
                QDateTime::fromString(jsonCoord["dateTime"].toString(), Qt::ISODate));

        //int index = -1;
        for(int j = 0; j < Devices.size(); j++)
        {
            //проверяем что устройство с таким маком уже известно
            if(jsonCoord["mac"].toString() == Devices[j].GetMacAddres())
            {
                Devices[j].UpdateCoord(c);
                flag = false;
                break;
            }
        }

        if(flag)//если известно, то обновляем координаты
        {           
            Device * d = new Device(jsonCoord["mac"].toString(), jsonCoord["name"].toString(), c);
            Devices.append(*d);
        }

    }
}

void MainWindow::ButtonSlot()
{
    ui->dateTimeEdit->setDateTime(QDateTime::currentDateTime());
    emit SignalFromButton();
}

void MainWindow::StartStopSlot()
{
    if(Timer->isActive())
    {
        Timer->stop();
        ui->PlayButt->setText("▶");
    }
    else
    {
        if(path == "" || Token == "")
        {
            QMessageBox::warning(this, "Ошибка","Сначала нужно авторизоваться и выбрать помещение для наблюдения!");
            return;
        }
        Timer->start(1000); // И запустим таймер
        ui->PlayButt->setText("▐▐");
    }

    emit SignalFromButton();
}

void MainWindow::slotTimerAlarm()
{
    /* Ежесекундно обновляем данные по текущему времени
     * Перезапускать таймер не требуется
     * */
    if(Devices.size() != 0)
    {
        //обновляем флаги отображения в объектах
        for(int i = 0; i < ui->tableWidget->rowCount(); i++)
        {
            Devices[i].SetDeviceVisible(ui->tableWidget->item(i, 2)->checkState());
            Devices[i].SetRouteVisible(ui->tableWidget->item(i, 3)->checkState());
            if(!Devices[i].SetRouteLength(ui->tableWidget->item(i, 3)->text().toInt()))
            {
                QMessageBox::warning(this, "Ошибка","Введена некорректная длина маршрута, введите от 0 до 500!");
            }
        }
    }

    //запрос данных
    sendToServer(QDateTime::fromString(ui->dateTimeEdit->text(), "dd.MM.yyyy hh:mm:ss").toString("yyyy-MM-dd hh:mm:ss"));

    UpdateList();
    //рисуем точки и линии
    DrawScene();//очищаем сцену
    for (int i = 0; i < Devices.size(); i++)
    {
        DrawSingleDevice(Devices[i]);
    }

    ui->dateTimeEdit->setDateTime(ui->dateTimeEdit->dateTime().addSecs(1));
}

void MainWindow::sendToServer(QString str)
{
    // Создаем менеджер доступа к сети
    QNetworkAccessManager manager;

    if(Token == "")
    {
        QMessageBox::warning(this, "Ошибка","Вы не авторизованы");
        Timer->stop();
        ui->PlayButt->setText("▶");
        return;
    }
    if(str == "")
    {
        QMessageBox::warning(this, "Ошибка","Не указано время");
        Timer->stop();
        ui->PlayButt->setText("▶");
        return;
    }
    if(areaId <= 0)
    {
        QMessageBox::warning(this, "Ошибка","Не выбрано помещение");
        Timer->stop();
        ui->PlayButt->setText("▶");
        return;
    }

    // Создаем URL-адрес с параметрами
    QUrl url("http://"+IP+":"+QString::number(port)+"/api/positions");
    QUrlQuery query;
    query.addQueryItem("token", Token);
    query.addQueryItem("time", str);
    query.addQueryItem("areaId", "1");
    url.setQuery(query);

    // Создаем запрос
    QNetworkRequest request(url);

    //qDebug() << url;
    // Отправляем GET-запрос
    QNetworkReply* reply = manager.get(request);

    // Ждем завершения запроса
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    // Проверяем статус запроса
    if (reply->error() == QNetworkReply::NoError) {
        // Получаем ответ от сервера
        QByteArray response = reply->readAll();
        // Обрабатываем ответ
        //qDebug() << "Response:" << response;
        unpackData(response);
    }
    else
    {
        // Возникла ошибка при выполнении запроса
        //qDebug() << "Error:" << reply->errorString();
    }

    // Освобождаем ресурсы
    reply->deleteLater();
    return;
}

void MainWindow::FileOpenSlot()
{
    if(Token == "")
    {
        QMessageBox::warning(this, "Ошибка","Вы не авторизованы");
        return;
    }

    AreaDialog.setParams(IP, port, Token);
    AreaDialog.setWindowTitle("Выбор помещения");
    AreaDialog.exec();
}
void MainWindow::AuthSlot()
{
    //dialog.setModal(true);
    dialog.setParams(IP, port);
    dialog.setWindowTitle("Авторизация");
    dialog.exec();
}
void MainWindow::onDataReceived(const QString& data)
{
    // Обработка полученных данных
    Token = data;
}

void MainWindow::onAreaChoosed(const QString& data)
{
    path = data.split("\n").first();
    areaId = data.split("\n").last().toInt();
    DrawScene();
    QImage image(path);
    PlanW = image.width();
}

void MainWindow::DrawScene()//отрисовка сцены
{
    scene = new QGraphicsScene(this);//создаем объект
    ui->graphicsView->setScene(scene);//добавляем на окно
    QImage *img_object = new QImage();//открываем план помещения
    img_object->load(path);
    QPixmap image = QPixmap::fromImage(*img_object);
    //масштабируем под размеры окна
    scaled_img = image.scaled(ui->graphicsView->geometry().width(), ui->graphicsView->geometry().height(), Qt::KeepAspectRatio);
    //подгоняем сцену под отмасштабированную картинку
    scene->addPixmap(scaled_img);
    scene->setSceneRect(scaled_img.rect());
}

void MainWindow::UpdateList()//обновление списка устройств
{
    while(ui->tableWidget->rowCount() != 0)//очищаем список
    {
        ui->tableWidget->removeRow(0);
    }

    //удаляем устройства по которым данные устарели
    foreach(Device d, Devices)
    {
        if(d.GetCurrentCoord().GetDateTime().addSecs(INTERVAL) < QDateTime::fromString(ui->dateTimeEdit->text(), "dd.MM.yyyy H:mm:ss"))
        {
            Devices.removeAt(Devices.indexOf(d));
        }
    }

//для каждого устройства делаем запись в списке
    for(int i = 0; i < Devices.size(); i++)
    {
        ui->tableWidget->insertRow(ui->tableWidget->rowCount());//вставляем строку
        //выделяем ячейку в цвет точки, отображающей данное устройства
        ui->tableWidget->setItem(i,0, new QTableWidgetItem());
        ui->tableWidget->item(i,0)->setBackground(QBrush(Devices[i].GetColor()));
        //выводим имя устройства
        ui->tableWidget->setItem(i,1, new QTableWidgetItem(Devices[i].GetName()));
        //чек бокс надо ли отображать это устройство на плане
        QTableWidgetItem *item = new QTableWidgetItem();
        if(Devices[i].GetDeviceVisible())
            item->setCheckState(Qt::Checked);
        else
            item->setCheckState(Qt::Unchecked);

        item->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(i, 2, item);
        //чек бокс "показать маршрут"
        item = new QTableWidgetItem();

        if(Devices[i].GetRouteVisible())
            item->setCheckState(Qt::Checked);
        else
            item->setCheckState(Qt::Unchecked);

        item->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(i, 3, item);
        ui->tableWidget->item(i, 3)->setText(QString::number(Devices[i].GetRouteLengh()));

        //ui->tableWidget->setItem(i,4, new QTableWidgetItem(QString::fromStdString(std::to_string(Devices[i].GetRouteLengh()))));
    }
}

void MainWindow::DrawSingleDevice(Device d)//отрисовка одного устройства
{
    if(d.GetDeviceVisible()&& d.GetCurrentCoord().GetDateTime().secsTo(QDateTime::fromString(ui->dateTimeEdit->text())) <= 15)//если его надо рисовать
    {
        QPen Pen_1 (d.GetColor());//контур
        QBrush Brush_1 (d.GetColor());//заполнение

        Pen_1.setWidth(3);//толщина контура
        int D=20;//диаметр точки

        //рисуем маршрут, если пользователь указал, что маршрут нужен
        if (d.GetRouteVisible())
        {
            QVector <Coordinate> points = d.GetRoute();
            points.push_front(d.GetCurrentCoord());

            for(int i = 0; i < points.size() - 1; i++)//последовательно рисуем линии от самой старой координаты к той где устройство сейчас
            {
                scene->addLine(points[i].GetX(), points[i].GetY(), points[i + 1].GetX(), points[i + 1].GetY(), Pen_1);
            }
        }
        //смещаем координаты так чтобы центр оказался в нужном месте
        //т.к. круг рисуется внутри квадрата где x y - координаты левого верхнего угла.
        //рисуем жирную точку там где устройство находится
        scene->addEllipse(d.GetCurrentCoord().GetX() - D/2, d.GetCurrentCoord().GetY() - D/2,D,D,Pen_1,Brush_1);
    }
}
void MainWindow::resizeEvent(QResizeEvent *event)//событие изменения размера окна
{
    if (path != "")
    {
        //запоминаем размеры до увеличения
        int w = scaled_img.width();
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

void MainWindow::on_showDevicesButton_clicked()
{
    if(path == "" || Token == "")
    {
        QMessageBox::warning(this, "Ошибка","Сначала нужно авторизоваться и выбрать помещение для наблюдения!");
        return;
    }

    //запрос данных
    sendToServer(QDateTime::fromString(ui->dateTimeEdit->text(), "dd.MM.yyyy hh:mm:ss").toString("yyyy-MM-dd hh:mm:ss"));

    UpdateList();
    //рисуем точки и линии
    DrawScene();//очищаем сцену
    for (int i = 0; i < Devices.size(); i++)
    {
        DrawSingleDevice(Devices[i]);
    }
}

void MainWindow::on_BackButt_clicked()
{
    ui->dateTimeEdit->setDateTime(ui->dateTimeEdit->dateTime().addSecs(-interval));
}

void MainWindow::on_ForwardButt_clicked()
{
    if(ui->dateTimeEdit->dateTime().addSecs(interval) < QDateTime::currentDateTime())
        ui->dateTimeEdit->setDateTime(ui->dateTimeEdit->dateTime().addSecs(interval));
}

void MainWindow::on_dateTimeEdit_dateTimeChanged(const QDateTime &dateTime)
{
    if(dateTime > QDateTime::currentDateTime())
        ui->dateTimeEdit->setDateTime(QDateTime::currentDateTime());
}

