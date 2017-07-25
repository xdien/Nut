/**************************************************************************
**
** This file is part of Nut project.
** https://github.com/HamedMasafi/Nut
**
** Nut is free software: you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** Nut is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with Nut.  If not, see <http://www.gnu.org/licenses/>.
**
**************************************************************************/

#include "dbgeography.h"

NUT_BEGIN_NAMESPACE

DbGeography::DbGeography(QObject *parent)
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
}

void DbGeography::setLongitude(qreal longitude)
{
    if (qFuzzyCompare(m_longitude, longitude))
        return;

    m_longitude = longitude;
}

QString DbGeography::toString()
{
    return QString("%1,%2").arg(longitude()).arg(latitude());
}

void DbGeography::fromString(const QString &s)
{
    QStringList parts = s.split(',');
    if (parts.count() == 2) {
        setLongitude(parts[0].toDouble());
        setLatitude(parts[1].toDouble());
    }
}

NUT_END_NAMESPACE
