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
