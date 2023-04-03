#include "device.h"

Device::Device(QString mac = "", QString name = "", QPoint currentCoord = QPoint(0, 0), int pointsCount = 10)
{
    CurrentCoord = currentCoord;
    Name = name;
    MacAddres = mac;
    PointsCount = pointsCount;
    Route.clear();
}
Device::Device()
{
    Name = "";
    MacAddres = "";
    CurrentCoord = QPoint(0,0);
    Route.clear();
}

//пересчет координат при изменении размеров окна
void Device::RecalcCoord(double coef)//коэф - отношение старого размера к новому
{
    CurrentCoord.setX(CurrentCoord.x() * coef);
    CurrentCoord.setY(CurrentCoord.y() * coef);

    for(int i = 0; i < Route.size(); i++)
    {
        Route[i].setX(Route[i].x() * coef);
        Route[i].setY(Route[i].y() * coef);
    }
}

void Device::UpdateCoord(QPoint point, bool NeedToRememberPrevCoord = false)
{
    //если работаем в режиме построения траектории, то сохраним предыдущую координату.
    if(NeedToRememberPrevCoord)
    {
        Route.push_front(CurrentCoord);
        //удаляем самую старую точку
        if(Route.size() > PointsCount)
        {
            Route.removeLast();
        }
    }

    CurrentCoord = point;

    return;
}

QPoint Device::GetCurrentCoord() const
{
    return CurrentCoord;
}
QVector <QPoint> Device::GetRoute() const
{
    return Route;
}
QString Device::GetMacAddres() const
{
    return MacAddres;
}
QString Device::GetName() const
{
    return Name;
}

void Device::SetPointsCount(int count)
{
    PointsCount = count;
    return;
}


