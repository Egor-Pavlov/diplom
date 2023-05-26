#include "chooseareadialog.h"
#include "ui_chooseareadialog.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QFileDialog>
#include <QJsonArray>
#include <QMessageBox>


#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrlQuery>
#include <QEventLoop>

ChooseAreaDialog::ChooseAreaDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChooseAreaDialog)
{
    ui->setupUi(this);
    ui->BuildingsList->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->AreaList->setSelectionMode(QAbstractItemView::SingleSelection);
    connect( ui->BuildingsList, &QListWidget::doubleClicked, this, &ChooseAreaDialog::onBuildingClicked);
}

//по двойному щелчку на здание
void ChooseAreaDialog::onBuildingClicked()
{

    ui->AreaList->clear();
    int id = Buildings[ui->BuildingsList->currentRow()].GetId();

    QUrl url("http://"+IP+":"+QString::number(port)+"/api/arealist");
    QUrlQuery query;
    query.addQueryItem("token", Token);
    query.addQueryItem("buildingid", QString::number(id));
    url.setQuery(query);
    MakeRequest(url);
}

void ChooseAreaDialog::unpackData(const QByteArray& data)
{
    //QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonString.toUtf8());
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
    QJsonObject jsonObj = jsonDoc.object();

    if(jsonObj["code"].toInt() != 200)
    {
        QMessageBox::warning(this, "Ошибка", RespCodes.value(jsonObj["code"].toInt()));

        return;
    }
    if (jsonObj.contains("buildings"))
    {
        QJsonArray jsonArray = jsonObj["buildings"].toArray();
        for (const QJsonValue& jsonValue : jsonArray)
        {
            QJsonObject jsonObject = jsonValue.toObject();

            QString address = jsonObject["address"].toString().replace("\n", " ");
            QString description = jsonObject["description"].toString().replace("\n", "");
            int id = jsonObject["id"].toInt();
            double latitude = jsonObject["latitude"].toDouble();
            double longitude = jsonObject["longitude"].toDouble();

            Building building(address, description, id, latitude, longitude);
            Buildings.append(building);
            ui->BuildingsList->addItem(building.GetInfo());
        }

        for(int i = 0; i < ui->BuildingsList->count(); i++)
        {
            QListWidgetItem *item = ui->BuildingsList->item(i);
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        }

    }
    if (jsonObj.contains("areas"))
    {
        QJsonArray jsonArray = jsonObj["areas"].toArray();
        for (const QJsonValue& jsonValue : jsonArray)
        {
            QJsonObject jsonObject = jsonValue.toObject();
            int id = jsonObject["id"].toInt();
            QString description = jsonObject["description"].toString();
            Areas.append(QPair(id, description));
            ui->AreaList->addItem(Areas.last().second);
        }
        for(int i = 0; i < ui->AreaList->count(); i++)
        {
            QListWidgetItem *item = ui->AreaList->item(i);
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        }
    }

    if (jsonObj.contains("image"))
    {
        QString filepath = Areas[ui->AreaList->currentRow()].second + ".jpg";
        // Получение закодированных данных изображения из объекта jsonObj
        QString encodedImageData = jsonObj["image"].toString();
        QByteArray imageData = QByteArray::fromBase64(encodedImageData.toLatin1());

        // Создание объекта QImage из распакованных данных
        QImage image;
        image.loadFromData(imageData);

        // Сохранение изображения в файл
        image.save(filepath);
        emit areaChoosed(filepath+"\n"+QString::number(Areas[ui->AreaList->currentRow()].first)); // Испускание сигнала с передаваемыми данными
        close();
    }
}

void ChooseAreaDialog::MakeRequest(const QUrl &url)
{

    // Создаем запрос
    QNetworkRequest request(url);

    qDebug() << url;
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
        qDebug() << "Error:" << reply->errorString();
    }

    // Освобождаем ресурсы
    reply->deleteLater();
    return;
}

void ChooseAreaDialog::setParams(const QString &ip, const int &port, const QString &token)
{
    this->IP = ip;
    this->port = port;
    this->Token = token;

    //запрос списка зданий
    // Создаем URL-адрес с параметрами
    QUrl url("http://"+IP+":"+QString::number(port)+"/api/buildings");
    QUrlQuery query;
    query.addQueryItem("token", Token);
    url.setQuery(query);
    MakeRequest(url);
}

void ChooseAreaDialog::sendData()
{
    QString path = "";
    emit areaChoosed(path); // Испускание сигнала с передаваемыми данными
}

void ChooseAreaDialog::closeEvent(QCloseEvent *event)
{
    ui->BuildingsList->clear();
    ui->AreaList->clear();
    //ui->passwordTextBox->setText("");
    QDialog::closeEvent(event);
}

ChooseAreaDialog::~ChooseAreaDialog()
{
    delete ui;
}

void ChooseAreaDialog::on_OKButton_clicked()
{

    if(ui->AreaList->currentRow() == -1)
    {
        QMessageBox::warning(this, "Ошибка","Необходимо выбрать помещение в списке");
        return;
    }

    int id = Areas[ui->AreaList->currentRow()].first;
    QUrl url("http://"+IP+":"+QString::number(port)+"/api/plan");
    QUrlQuery query;
    query.addQueryItem("token", Token);
    query.addQueryItem("areaId", QString::number(id));
    url.setQuery(query);
    MakeRequest(url);
}

