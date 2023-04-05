#include "coordinate.h"

Coordinate::Coordinate()
{
    Point = QPoint(0,0);
    Time = QTime(0,0,0,0);
}
Coordinate::Coordinate(QPoint point = QPoint(0,0), QTime time = QTime(0,0,0,0))
{
    Point = point;
    Time = time;
}
Coordinate::Coordinate(int x = 0, int y = 0, QTime time = QTime(0,0,0,0))
{
    Point = QPoint(x, y);
    Time = time;
}
int Coordinate::GetX() const
{
    return Point.x();
}
int Coordinate::GetY() const
{
    return Point.y();
}
QTime Coordinate::GetTime() const
{
    return Time;
}
void Coordinate::SetPoint(QPoint point)
{
    Point = point;
}
void Coordinate::SetPoint(int x, int y)
{
    Point = QPoint(x, y);
}

