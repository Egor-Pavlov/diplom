#ifndef BUILDING_H
#define BUILDING_H
#include <QString>

class Building
{
private:
    QString address;
    QString description;
    int id;
    double latitude;
    double longitude;
public:
    Building(const QString& address, const QString& description, int id, double latitude, double longitude)
            : address(address), description(description), id(id), latitude(latitude), longitude(longitude) {
        }

    int GetId() const
    {
        return id;
    }

    QString GetInfo() const
    {
        QString info;
        info += address + " ";
        info += description;
        return info;
    }
};

#endif // BUILDING_H
