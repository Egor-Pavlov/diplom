#ifndef DEVICE_H
#define DEVICE_H
#include "QString"
#include <QPoint>
#include <QVector>
#include "coordinate.h"

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

    void UpdateCoord(Coordinate point, bool NeedToRemeberPrevCoord);

    //пересчет координат при изменении размеров окна
    void RecalcCoord(double coef);//коэф - отношение старого размера к новому

    Coordinate GetCurrentCoord() const;
    QVector <Coordinate> GetRoute() const;
    QString GetMacAddres() const;
    QString GetName() const;
    QColor GetColor() const;

    void ClearRoute();
    void SetPointsCount(int count);
    bool operator==(const Device &d) const;

private:

    QColor Color;
    QString MacAddres;
    QString Name;
    Coordinate CurrentCoord;
    //чем больше индекс точки в массиве, тем она старше. текущей точки в массиве НЕТ
    QVector <Coordinate> Route;
    int PointsCount;
};

#endif // DEVICE_H
