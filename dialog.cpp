#include "dialog.h"
#include "ui_dialog.h"
#include <QCloseEvent>
#include <QMessageBox>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFileDialog>
#include <QJsonArray>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrlQuery>
#include <QEventLoop>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    ui->passwordTextBox->setEchoMode(QLineEdit::Password);
    // Подключение слота к сигналу изменения состояния флажка
    connect(ui->showPassword, &QCheckBox::stateChanged, [=](int state)
    {
        if (state == Qt::Checked)
        {
            // Если флажок выбран, показываем пароль
            ui->passwordTextBox->setEchoMode(QLineEdit::Normal);
        } else
        {
            // Если флажок не выбран, скрываем пароль
            ui->passwordTextBox->setEchoMode(QLineEdit::Password);
        }
    });

}
void Dialog::setParams(const QString &ip, const int &port)
{
    this->IP = ip;
    this->port = port;
}

void Dialog::sendData()
{
    QString data = "";
    emit dataReady(data); // Испускание сигнала с передаваемыми данными
}

void Dialog::closeEvent(QCloseEvent *event)
{
    ui->loginTextBox->setText("");
    ui->passwordTextBox->setText("");
    QDialog::closeEvent(event);
}

//проверяем наличие sql инъекций в логине или пароле
bool hasSqlInjection(const QString& input)
{
    // Список ключевых слов SQL
    QStringList sqlKeywords = {
        "SELECT", "INSERT", "UPDATE", "DELETE",
        "DROP", "CREATE", "ALTER", "TRUNCATE"
        // Добавьте другие ключевые слова SQL по необходимости
    };
    // Проверка наличия ключевых слов SQL во входной строке
    foreach (const QString& keyword, sqlKeywords) {
        if (input.contains(keyword, Qt::CaseInsensitive)) {
            return true; // Найдено ключевое слово SQL
        }
    }
    // Проверка наличия символов, используемых для SQL инъекций
    QStringList sqlSpecialChars = {"'", "\"", ";", "--", "/*", "*/"};
    foreach (const QString& specialChar, sqlSpecialChars) {
        if (input.contains(specialChar)) {
            return true; // Найден символ SQL инъекции
        }
    }
    // Если не найдены ключевые слова SQL или символы инъекции
    return false;
}
void Dialog::on_okButton_clicked()
{
    if(hasSqlInjection(ui->loginTextBox->text()) || hasSqlInjection(ui->passwordTextBox->text()))
    {
        QMessageBox::warning(this, "упс","Были введены недопустимые символы");
        return;
    }
    try
    {
        // Создаем менеджер доступа к сети
        QNetworkAccessManager manager;

        // Создаем URL-адрес с параметрами
        QUrl url("http://"+IP+":"+QString::number(port)+"/api/token");
        QUrlQuery query;
        query.addQueryItem("username", ui->loginTextBox->text());
        query.addQueryItem("password", ui->passwordTextBox->text());
        url.setQuery(query);

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
        if (reply->error() == QNetworkReply::NoError)
        {
            // Получаем ответ от сервера
            QByteArray response = reply->readAll();
            // Обрабатываем ответ
            //qDebug() << "Response:" << response;
            QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
            QJsonObject jsonObj = jsonDoc.object();

            if(jsonObj["code"].toInt() != 200)
            {
                QMessageBox::warning(nullptr, "Ошибка", "Вы не авторизованы");
                return;
            }
            else
            {
                QString token = jsonObj["token"].toString();
                emit dataReady(token); // Испускание сигнала с передаваемыми данными
                // Освобождаем ресурсы
                reply->deleteLater();
                //закрываем окно авторизации
                close();
            }
        }
        else
        {
            // Возникла ошибка при выполнении запроса
            qDebug() << "Error:" << reply->errorString();
        }
        // Освобождаем ресурсы
        reply->deleteLater();
    }
    catch(...)
    {
        return;
    }
}

Dialog::~Dialog()
{
    delete ui;
}
