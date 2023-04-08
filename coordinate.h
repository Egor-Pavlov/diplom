#ifndef COORDINATE_H
#define COORDINATE_H

#include <Qpoint>
#include <QTime>

class Coordinate
{
public:
    Coordinate();
    Coordinate(QPoint point, QDateTime DateTime);
    Coordinate(int x, int y, QDateTime DateTime);
    int GetX() const;
    int GetY() const;
    QDateTime GetDateTime() const;
    void SetPoint(QPoint point);
    void SetPoint(int x, int y);

private:
    QPoint Point;
    //время в которое координата была зафиксирована
    QDateTime DateTime;

};

#endif // COORDINATE_H
