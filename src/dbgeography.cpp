#include "dbgeography.h"

NUT_BEGIN_NAMESPACE

DbGeography::DbGeography(QObject *parent) //: QObject(parent)
{

}

DbGeography::DbGeography(const DbGeography &other)
{
    setLatitude(other.latitude());
    setLongitude(other.longitude());
}

qreal DbGeography::latitude() const
{
    return m_latitude;
}

qreal DbGeography::longitude() const
{
    return m_longitude;
}

void DbGeography::setLatitude(qreal latitude)
{
    if (qFuzzyCompare(m_latitude, latitude))
        return;

    m_latitude = latitude;
//    emit latitudeChanged(latitude);
}

void DbGeography::setLongitude(qreal longitude)
{
    if (qFuzzyCompare(m_longitude, longitude))
        return;

    m_longitude = longitude;
//    emit longitudeChanged(longitude);
}

NUT_END_NAMESPACE
