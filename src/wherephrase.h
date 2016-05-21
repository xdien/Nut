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


QT_BEGIN_NAMESPACE

class SqlGeneratorBase;
struct PhraseData{
    enum Condition
    {
        Equal = 0,
        Less,
        LessEqual,
        Null,
        In,
        Like,

        NotEqual = 10,
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
        Divide
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

    QString operatorString() const;
    QString escapeVariant() const;
    QString command(SqlGeneratorBase *generator) const;
};

class FieldPhrase{
    PhraseData *data;
    bool willDeleteData;
public:

    QString text;

    FieldPhrase(const char *className, const char* s);

    FieldPhrase(PhraseData *l);
    FieldPhrase(PhraseData *l, PhraseData::Condition o);
    FieldPhrase(PhraseData *l, PhraseData::Condition o, PhraseData *r);
    FieldPhrase(PhraseData *l, PhraseData::Condition o, QVariant r);

    ~FieldPhrase();

    QString command(SqlGeneratorBase *generator);

    void deleteData(PhraseData *d);

    FieldPhrase operator ==(const FieldPhrase &other);
    FieldPhrase operator !=(const FieldPhrase &other);
    FieldPhrase operator <(const FieldPhrase &other);
    FieldPhrase operator >(const FieldPhrase &other);
    FieldPhrase operator <=(const FieldPhrase &other);
    FieldPhrase operator >=(const FieldPhrase &other);

    FieldPhrase operator =(const FieldPhrase &other);

    FieldPhrase operator +(const FieldPhrase &other);
    FieldPhrase operator -(const FieldPhrase &other);
    FieldPhrase operator *(const FieldPhrase &other);
    FieldPhrase operator /(const FieldPhrase &other);

    FieldPhrase operator &&(const FieldPhrase &other);
    FieldPhrase operator ||(const FieldPhrase &other);

    FieldPhrase operator &(const FieldPhrase &other);

    FieldPhrase operator ,(const FieldPhrase &other);

    FieldPhrase operator !();

    FieldPhrase operator ==(const QVariant &other);
    FieldPhrase operator !=(const QVariant &other);
    FieldPhrase operator <(const QVariant &other);
    FieldPhrase operator >(const QVariant &other);
    FieldPhrase operator <=(const QVariant &other);
    FieldPhrase operator >=(const QVariant &other);

    FieldPhrase operator =(const QVariant &other);

    FieldPhrase isNull();
    FieldPhrase in(QVariantList list);
    FieldPhrase in(QStringList list);
    FieldPhrase like(QString pattern);
};

QT_END_NAMESPACE

#endif // PHRASE_H
