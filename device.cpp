#include "device.h"

Device::Device(QString mac = "", QString name = "", Coordinate currentCoord = Coordinate(), int pointsCount = 10)
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
    CurrentCoord = Coordinate();
    Route.clear();
}

//пересчет координат при изменении размеров окна
void Device::RecalcCoord(double coef)//коэф - отношение старого размера к новому
{
    CurrentCoord.SetPoint(CurrentCoord.GetX() * coef, CurrentCoord.GetY() * coef);

    for(int i = 0; i < Route.size(); i++)
    {
        Route[i].SetPoint(Route[i].GetX() * coef, Route[i].GetY() * coef);
    }
}

void Device::UpdateCoord(Coordinate point, bool NeedToRememberPrevCoord = false)
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

bool Device::operator==(const Device &d) const
{
    return (d.GetName() == Name && d.GetMacAddres() == MacAddres);
}

Coordinate Device::GetCurrentCoord() const
{
    return CurrentCoord;
}
QVector <Coordinate> Device::GetRoute() const
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

void Device::ClearRoute()
{
    Route.clear();
    return;
}

