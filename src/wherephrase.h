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

class WherePhrase{
protected:
    PhraseData *data;
    bool willDeleteData;

public:

    QString text;

    WherePhrase(const char *className, const char* s);

    WherePhrase(PhraseData *l);
    WherePhrase(PhraseData *l, PhraseData::Condition o);
    WherePhrase(PhraseData *l, PhraseData::Condition o, PhraseData *r);
    WherePhrase(PhraseData *l, PhraseData::Condition o, QVariant r);

    ~WherePhrase();

    QString command(SqlGeneratorBase *generator);

    void deleteData(PhraseData *d);

    WherePhrase operator ==(const WherePhrase &other);
    WherePhrase operator !=(const WherePhrase &other);
    WherePhrase operator <(const WherePhrase &other);
    WherePhrase operator >(const WherePhrase &other);
    WherePhrase operator <=(const WherePhrase &other);
    WherePhrase operator >=(const WherePhrase &other);

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


};

class FieldPhrase: public WherePhrase{
public:
    FieldPhrase(const char *className, const char* s);

    WherePhrase operator &(const QVariant &other);

    WherePhrase operator =(const QVariant &other);
    WherePhrase operator !();

    WherePhrase isNull();
    WherePhrase in(QVariantList list);
    WherePhrase in(QStringList list);
    WherePhrase like(QString pattern);
};

QT_END_NAMESPACE

#endif // PHRASE_H
