#ifndef DEVICE_H
#define DEVICE_H
#include "QString"
#include <QPoint>
#include <QVector>
#include "coordinate.h"
#include <device.h>
#include <QColor>

//класс для точек:
//	мак
//	имя
//	координаты относительно воображаемого центра
//	несколько предыдущих координат

// время?

class Device
{
public:
    Device();
    Device(QString mac, QString name, Coordinate currentCoord, int count);

    void UpdateCoord(Coordinate point);

    //пересчет координат при изменении размеров окна
    void RecalcCoord(double coef);//коэф - отношение старого размера к новому

    Coordinate GetCurrentCoord() const;
    QVector <Coordinate> GetRoute() const;
    QString GetMacAddres() const;
    QString GetName() const;
    QColor GetColor() const;
    bool GetDeviceVisible() const;
    bool GetRouteVisible() const;

    void SetDeviceVisible(bool vis);
    void SetRouteVisible(bool vis);
    void ClearRoute();
    void SetPointsCount(int count);
    bool operator==(const Device &d) const;

private:
    //цвет точки
    QColor Color;
    //мак устройства
    QString MacAddres;
    //имя устройства
    QString Name;
    //текущая координата
    Coordinate CurrentCoord;
    //чем больше индекс точки в массиве, тем она старше. текущей точки в массиве НЕТ
    QVector <Coordinate> Route;
    //длинна маршрута
    int PointsCount;
    //отображать на карте или нет
    bool IsVisible = true;
    //показывать маршрут или нет
    bool RouteVisible = false;
};

#endif // DEVICE_H
