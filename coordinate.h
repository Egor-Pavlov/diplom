#ifndef COORDINATE_H
#define COORDINATE_H

#include <Qpoint>
#include <QTime>

class Coordinate
{
public:
    Coordinate();
    Coordinate(QPoint point, QTime time);
    Coordinate(int x, int y, QTime time);
    int GetX() const;
    int GetY() const;
    QTime GetTime() const;
    void SetPoint(QPoint point);
    void SetPoint(int x, int y);

private:
    QPoint Point;
    //время в которое координата была зафиксирована
    QTime Time;

};

#endif // COORDINATE_H
