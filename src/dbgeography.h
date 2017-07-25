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

#ifndef DBGEOGRAPHY_H
#define DBGEOGRAPHY_H

#include "defines.h"
#include <QtCore/QObject>
#include <QtCore/qglobal.h>

NUT_BEGIN_NAMESPACE

class NUT_EXPORT DbGeography //: public QObject
{
    qreal m_latitude;
    qreal m_longitude;

public:
    explicit DbGeography(QObject *parent = 0);
    DbGeography(const DbGeography &other);

    qreal latitude() const;
    qreal longitude() const;

    void setLatitude(qreal latitude);
    void setLongitude(qreal longitude);

    QString toString();
    void fromString(const QString &s);
};

NUT_END_NAMESPACE

#ifdef NUT_NAMESPACE
    Q_DECLARE_METATYPE(NUT_NAMESPACE::DbGeography)
#else
    Q_DECLARE_METATYPE(DbGeography)
#endif

#endif // DBGEOGRAPHY_H
