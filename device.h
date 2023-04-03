#ifndef DEVICE_H
#define DEVICE_H

#include "QString"
#include <QPoint>
#include <QVector>

//класс для точек:
//	мак
//	имя
//	координаты относительно воображаемого центра
//	несколько предыдущих координат


class Device
{
public:
    Device();
    Device(QString mac, QString name, QPoint currentCoord, int count);

    void UpdateCoord(QPoint point, bool NeedToRemeberPrevCoord);

    //пересчет координат при изменении размеров окна
    void RecalcCoord(double coef);//коэф - отношение старого размера к новому

    QPoint GetCurrentCoord() const;
    QVector <QPoint> GetRoute() const;
    QString GetMacAddres() const;
    QString GetName() const;

    void SetPointsCount(int count);

private:
    QString MacAddres;
    QString Name;
    QPoint CurrentCoord;
    //чем больше индекс точки в массиве, тем она старше. текущей точки в массиве НЕТ
    QVector <QPoint> Route;
    int PointsCount;



};

#endif // DEVICE_H
