#ifndef DBGEOGRAPHY_H
#define DBGEOGRAPHY_H

#include "defines.h"
#include <QtCore/QObject>
#include <QtCore/qglobal.h>

NUT_BEGIN_NAMESPACE

class NUT_EXPORT DbGeography //: public QObject
{
//    Q_PROPERTY(qreal latitude READ latitude WRITE setLatitude NOTIFY
//                   latitudeChanged)
//    Q_PROPERTY(qreal longitude READ longitude WRITE setLongitude NOTIFY
//                   longitudeChanged)
    qreal m_latitude;
    qreal m_longitude;

public:
    explicit DbGeography(QObject *parent = 0);
    DbGeography(const DbGeography &other);

    qreal latitude() const;
    qreal longitude() const;

//signals:
//    void latitudeChanged(qreal latitude);
//    void longitudeChanged(qreal longitude);

//public slots:
    void setLatitude(qreal latitude);
    void setLongitude(qreal longitude);
};

NUT_END_NAMESPACE

#ifdef NUT_NAMESPACE
    Q_DECLARE_METATYPE(NUT_NAMESPACE::DbGeography)
#else
    Q_DECLARE_METATYPE(DbGeography)
#endif

#endif // DBGEOGRAPHY_H
