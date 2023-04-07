#include "device.h"

Device::Device(QString mac = "", QString name = "", Coordinate currentCoord = Coordinate(), int pointsCount = 10)
{
    CurrentCoord = currentCoord;
    Name = name;
    MacAddres = mac;
    PointsCount = pointsCount;
    Route.clear();

    QStringList color = MacAddres.split(':');

    int r = (color[0].toInt(nullptr, 16) + color[1].toInt(nullptr, 16))/2;
    int g = (color[2].toInt(nullptr, 16) + color[3].toInt(nullptr, 16))/2;
    int b = (color[4].toInt(nullptr, 16) + color[5].toInt(nullptr, 16))/2;

    Color = QColor(r, g, b);//контур
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

void Device::UpdateCoord(Coordinate point)
{

    //если работаем в режиме построения траектории, то сохраним предыдущую координату.
    if(RouteVisible)
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

bool Device::GetDeviceVisible() const
{
    return IsVisible;
}
bool Device::GetRouteVisible() const
{
    return RouteVisible;
}

void Device::SetPointsCount(int count)
{
    PointsCount = count;
    return;
}
void Device::SetDeviceVisible(bool vis)
{
    IsVisible = vis;
    return;
}
void Device::SetRouteVisible(bool vis)
{
    RouteVisible = vis;
    if(vis == false)
    {
        Route.clear();
    }
    return;
}

void Device::ClearRoute()
{
    Route.clear();
    return;
}
QColor Device::GetColor() const
{
    return Color;
}

