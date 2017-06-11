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

#ifndef PHRASE_H
#define PHRASE_H

#include <QtCore/qglobal.h>

#include <QVariant>
#include <QDate>
#include <QDateTime>
#include <QTime>
#include <QPoint>
#include <QSharedPointer>
#include "defines.h"
#include "dbgeography.h"

NUT_BEGIN_NAMESPACE

class SqlGeneratorBase;
class PhraseData{
public:
    enum Condition
    {
        NotAssign = 0,
        Equal,
        Less,
        LessEqual,
        Null,
        In,
        Like,

        Not = 10,
        NotEqual,
        GreaterEqual,
        Greater,
        NotNull,
        NotIn,
        NotLike,

        And = 20,
        Or,


        Append,
        Set,

        Add,
        Minus,
        Multiple,
        Divide,

        //special types
        Distance
    };

    enum Type{
        Field,
        WithVariant,
        WithOther,
        WithoutOperand
    };
    Type type;

    Condition operatorCond;

    QString text;
    const PhraseData *left;
    const PhraseData *right;
    QVariant operand;

    PhraseData(const char *className, const char* s);
    PhraseData(PhraseData *l, Condition o);
    PhraseData(PhraseData *l, Condition o, const PhraseData *r);
    PhraseData(PhraseData *l, Condition o, QVariant r);

    ~PhraseData();
};

class WherePhrase{
protected:
    PhraseData *_data;
    QSharedPointer<PhraseData> _dataPointer;

public:
    WherePhrase(const char *className, const char* s);

    WherePhrase(const WherePhrase &l);
    WherePhrase(WherePhrase *l);
    WherePhrase(WherePhrase *l, PhraseData::Condition o);
    WherePhrase(WherePhrase *l, PhraseData::Condition o, WherePhrase *r);
    WherePhrase(WherePhrase *l, PhraseData::Condition o, QVariant r);

    ~WherePhrase();

    WherePhrase operator ==(const WherePhrase &other);
    WherePhrase operator !=(const WherePhrase &other);
    WherePhrase operator <(const WherePhrase &other);
    WherePhrase operator >(const WherePhrase &other);
    WherePhrase operator <=(const WherePhrase &other);
    WherePhrase operator >=(const WherePhrase &other);

    WherePhrase operator !();
    WherePhrase operator =(const WherePhrase &other);

    WherePhrase operator +(const WherePhrase &other);
    WherePhrase operator -(const WherePhrase &other);
    WherePhrase operator *(const WherePhrase &other);
    WherePhrase operator /(const WherePhrase &other);

    WherePhrase operator &&(const WherePhrase &other);
    WherePhrase operator ||(const WherePhrase &other);

    WherePhrase operator &(const WherePhrase &other);


    WherePhrase operator ==(const QVariant &other);
    WherePhrase operator !=(const QVariant &other);
    WherePhrase operator <(const QVariant &other);
    WherePhrase operator >(const QVariant &other);
    WherePhrase operator <=(const QVariant &other);
    WherePhrase operator >=(const QVariant &other);


    PhraseData *data() const;
};

template<typename T>
class FieldPhrase: public WherePhrase{
public:
    FieldPhrase(const char *className, const char* s);

    WherePhrase operator =(const WherePhrase &other);
    WherePhrase operator =(const QVariant &other);
    WherePhrase operator +(const QVariant &other);
    WherePhrase operator !();

    WherePhrase isNull();
    WherePhrase in(QList<T> list);
//    WherePhrase in(QStringList list);
    WherePhrase like(QString pattern);
};


template<typename T>
Q_OUTOFLINE_TEMPLATE FieldPhrase<T>::FieldPhrase(const char *className, const char *s) : WherePhrase(className, s)
{
//    qDebug() << "(" << this << ")" << "FieldPhrase ctor" << className << s;
}

template<typename T>
Q_OUTOFLINE_TEMPLATE WherePhrase FieldPhrase<T>::operator =(const QVariant &other)
{
    return WherePhrase(this, PhraseData::Set, other);
}

template<typename T>
Q_OUTOFLINE_TEMPLATE WherePhrase FieldPhrase<T>::operator =(const WherePhrase &other)
{
    return WherePhrase(this, PhraseData::Set, &other);
}

template<typename T>
Q_OUTOFLINE_TEMPLATE WherePhrase FieldPhrase<T>::operator+(const QVariant &other)
{
    return WherePhrase(this, PhraseData::Add, other);
}

template<typename T>
Q_OUTOFLINE_TEMPLATE WherePhrase FieldPhrase<T>::operator !()
{
    if(_data->operatorCond < 20)
        _data->operatorCond = (PhraseData::Condition)((_data->operatorCond + 10) % 20);
    else
        qFatal("Operator ! can not aplied to non condition statements");

    return this;//WherePhrase(this, PhraseData::Not);
}

template<typename T>
Q_OUTOFLINE_TEMPLATE WherePhrase FieldPhrase<T>::isNull(){
    return WherePhrase(this, PhraseData::Null);
}

template<typename T>
Q_OUTOFLINE_TEMPLATE WherePhrase FieldPhrase<T>::in(QList<T> list)
{
    QVariantList vlist;
    foreach (T t, list)
        vlist.append(QVariant::fromValue(t));

    return WherePhrase(this, PhraseData::In, vlist);
}

//template<typename T>
//Q_OUTOFLINE_TEMPLATE WherePhrase FieldPhrase<T>::in(QStringList list)
//{
//    return WherePhrase(this, PhraseData::In, list);
//}

template<typename T>
Q_OUTOFLINE_TEMPLATE WherePhrase FieldPhrase<T>::like(QString pattern)
{
    return WherePhrase(this, PhraseData::Like, pattern);
}


// Custom types
template<>
class FieldPhrase<DbGeography>: public WherePhrase {
public:
    FieldPhrase(const char *className, const char* s) : WherePhrase(className, s){

    }

    WherePhrase distance(const DbGeography &geo) {
        return WherePhrase(this, PhraseData::Distance, QVariant::fromValue(geo));
    }
};

// Custom types
template<>
class FieldPhrase<QPoint>: public WherePhrase {
public:
    FieldPhrase(const char *className, const char* s) : WherePhrase(className, s){

    }

    WherePhrase distance(const QPoint &geo) {
        return WherePhrase(this, PhraseData::Distance, QVariant::fromValue(geo));
    }
    WherePhrase operator =(const QPoint &other) {
        return WherePhrase(this, PhraseData::Set, other);
    }
};

// Custom types
template<>
class FieldPhrase<QPointF>: public WherePhrase {
public:
    FieldPhrase(const char *className, const char* s) : WherePhrase(className, s){

    }

    WherePhrase distance(const QPointF &geo) {
        return WherePhrase(this, PhraseData::Distance, QVariant::fromValue(geo));
    }
    WherePhrase operator =(const QPointF &other) {
        return WherePhrase(this, PhraseData::Set, other);
    }
};

template<>
class FieldPhrase<bool>: public WherePhrase{
public:
    FieldPhrase(const char *className, const char* s) : WherePhrase(className, s){

    }

    WherePhrase operator ==(const bool &other) {
        return WherePhrase(this, PhraseData::Equal, other ? 1 : 0);
    }

    WherePhrase operator =(const bool &other) {
        return WherePhrase(this, PhraseData::Set, other ? 1 : 0);
    }

    WherePhrase operator !() {
        return WherePhrase(this, PhraseData::Equal, 0);
    }
};

NUT_END_NAMESPACE

#endif // PHRASE_H
