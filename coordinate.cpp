#include "coordinate.h"

Coordinate::Coordinate()
{
    Point = QPoint(0,0);
    DateTime = QDateTime::currentDateTime();
}
Coordinate::Coordinate(QPoint point = QPoint(0,0), QDateTime dateTime = QDateTime::currentDateTime())
{
    Point = point;
    DateTime = dateTime;
}
Coordinate::Coordinate(int x = 0, int y = 0, QDateTime dateTime = QDateTime::currentDateTime())
{
    Point = QPoint(x, y);
    DateTime = dateTime;
}
int Coordinate::GetX() const
{
    return Point.x();
}
int Coordinate::GetY() const
{
    return Point.y();
}
QDateTime Coordinate::GetDateTime() const
{
    return DateTime;
}
void Coordinate::SetPoint(QPoint point)
{
    Point = point;
}
void Coordinate::SetPoint(int x, int y)
{
    Point = QPoint(x, y);
}

