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

#include <QDataStream>
#include <QDebug>

#include "wherephrase.h"

QT_BEGIN_NAMESPACE

PhraseData::PhraseData(const char *className, const char *s){
    text = QString(className) + "." + s;
    type = Field;
}

PhraseData::PhraseData(PhraseData *l, PhraseData::Condition o) : left(l){
    operatorCond = o;
    type = WithoutOperand;
}

PhraseData::PhraseData(PhraseData *l, PhraseData::Condition o, const PhraseData *r) : left(l), right(r){
    operatorCond = o;
    type = WithOther;
}

PhraseData::PhraseData(PhraseData *l, PhraseData::Condition o, QVariant r) : left(l), operand(r){
    operatorCond = o;
    type = WithVariant;
}

PhraseData::~PhraseData(){
    //        if(type == WithOther){
    //            delete left;
    //            delete right;
    //        }
    //        if(type == WithVariant){
    ////            qDebug() << operator
    //                delete left;
}

QString PhraseData::operatorString() const
{
    switch (operatorCond){
    case PhraseData::Equal:
        return "=";
    case PhraseData::NotEqual:
        return "<>";
    case PhraseData::Less:
        return "<";
    case PhraseData::Greater:
        return ">";
    case PhraseData::LessEqual:
        return "<=";
    case PhraseData::GreaterEqual:
        return ">=";
    case PhraseData::Null:
        return "IS NULL";

    case PhraseData::NotNull:
        return "IS NOT NULL";

    case PhraseData::In:
        return "IN";

    case PhraseData::NotIn:
        return "NOT IN";

    case PhraseData::And:
        return "AND";
    case PhraseData::Or:
        return "OR";

    case PhraseData::Like:
        return "LIKE";
    case PhraseData::NotLike:
        return "NOT LIKE";

    case PhraseData::Add:
        return "+";
    case PhraseData::Minus:
        return "-";
    case PhraseData::Multiple:
        return "*";
    case PhraseData::Divide:
        return "/";

    case PhraseData::Set:
        return "=";

    case PhraseData::Append:
        return ",";
    }

    return QString("<FAIL>");
}

QString PhraseData::escapeVariant() const
{
    switch (operand.type()) {
    case QVariant::Int:
    case QVariant::Double:
        return operand.toString();
        break;

    case QVariant::String:
        return "'" + operand.toString() + "'";

    case QVariant::DateTime:
        return "'" + operand.toDateTime().toString() + "'";

    case QVariant::Date:
        return "'" + operand.toDate().toString() + "'";

    case QVariant::Time:
        return "'" + operand.toTime().toString() + "'";

    case QVariant::StringList:
    case QVariant::List:
        return "['" + operand.toStringList().join("', '") + "']";

    case QVariant::Invalid:
        return "<FAIL>";

    default:
        return "";
    }
}

QString PhraseData::command(SqlGeneratorBase *generator) const
{
    QString ret = "";

    switch(type){
    case Field:
        ret = text;
        break;

    case WithVariant:
        ret = left->command(generator) + " " + operatorString() + " " + escapeVariant();
        break;

    case WithOther:
        ret = left->command(generator) + " " + operatorString() + " " + right->command(generator);
        break;

    case WithoutOperand:
        ret = left->command(generator) + " " + operatorString();
        break;
    }

    if(operatorCond == PhraseData::And || operatorCond == PhraseData::Or)
        ret = "(" + ret + ")";

    return ret;
}


FieldPhrase::FieldPhrase(const char *className, const char *s) : willDeleteData(false)
{
    data = new PhraseData(className, s);
    text = QString(className) + "." + s;
}

FieldPhrase::FieldPhrase(PhraseData *l) : willDeleteData(false)
{
    data = l;
}

FieldPhrase::FieldPhrase(PhraseData *l, PhraseData::Condition o) : willDeleteData(false)
{
    data = new PhraseData(l, o);
}


FieldPhrase::FieldPhrase(PhraseData *l, PhraseData::Condition o, PhraseData *r) : willDeleteData(false)
{
    data = new PhraseData(l, o, r);
}

FieldPhrase::FieldPhrase(PhraseData *l, PhraseData::Condition o, QVariant r) : willDeleteData(false)
{
    data = new PhraseData(l, o, r);
}

FieldPhrase::~FieldPhrase()
{
//    if(willDeleteData)
//        delete data;
}


QString FieldPhrase::command(SqlGeneratorBase *generator)
{
    willDeleteData = true;
    return data->command(generator);
}

void FieldPhrase::deleteData(PhraseData *d)
{
    deleteData(d);
    if(d->type == PhraseData::WithOther){
        delete d->left;
        delete d->right;
    }
    if(d->type == PhraseData::WithVariant)
        delete d->left;
}

FieldPhrase FieldPhrase::operator ==(const FieldPhrase &other){
    return FieldPhrase(this->data, PhraseData::Equal, other.data);
}

FieldPhrase FieldPhrase::operator !=(const FieldPhrase &other){
    return FieldPhrase(this->data, PhraseData::NotEqual, other.data);
}

FieldPhrase FieldPhrase::operator <(const FieldPhrase &other){
    return FieldPhrase(this->data, PhraseData::Less, other.data);
}

FieldPhrase FieldPhrase::operator >(const FieldPhrase &other){
    return FieldPhrase(this->data, PhraseData::Greater, other.data);
}

FieldPhrase FieldPhrase::operator <=(const FieldPhrase &other){
    return FieldPhrase(this->data, PhraseData::LessEqual, other.data);
}

FieldPhrase FieldPhrase::operator >=(const FieldPhrase &other){
    return FieldPhrase(this->data, PhraseData::GreaterEqual, other.data);
}

FieldPhrase FieldPhrase::operator =(const FieldPhrase &other)
{
    return FieldPhrase(this->data, PhraseData::Set, other.data);
}

FieldPhrase FieldPhrase::operator +(const FieldPhrase &other){
    return FieldPhrase(this->data, PhraseData::Add, other.data);
}

FieldPhrase FieldPhrase::operator -(const FieldPhrase &other){
    return FieldPhrase(this->data, PhraseData::Minus, other.data);
}

FieldPhrase FieldPhrase::operator *(const FieldPhrase &other){
    return FieldPhrase(this->data, PhraseData::Multiple, other.data);
}

FieldPhrase FieldPhrase::operator /(const FieldPhrase &other){
    return FieldPhrase(this->data, PhraseData::Divide, other.data);
}

FieldPhrase FieldPhrase::operator &&(const FieldPhrase &other){
    return FieldPhrase(this->data, PhraseData::And, other.data);
}

FieldPhrase FieldPhrase::operator ||(const FieldPhrase &other){
    return FieldPhrase(this->data, PhraseData::Or, other.data);
}

FieldPhrase FieldPhrase::operator &(const FieldPhrase &other)
{
    return FieldPhrase(this->data, PhraseData::Append, other.data);
}

FieldPhrase FieldPhrase::operator ,(const FieldPhrase &other)
{
    return FieldPhrase(this->data, PhraseData::Append, other.data);
}

FieldPhrase FieldPhrase::operator !(){
    if(data->operatorCond < 20)
        data->operatorCond = (PhraseData::Condition)((data->operatorCond + 10) % 20);
    else
        qFatal("Operator ! can not aplied to non condition statements");

    return FieldPhrase(data);
}

FieldPhrase FieldPhrase::operator ==(const QVariant &other){
    return FieldPhrase(this->data, PhraseData::Equal, other);
}

FieldPhrase FieldPhrase::operator !=(const QVariant &other){
    return FieldPhrase(this->data, PhraseData::NotEqual, other);
}

FieldPhrase FieldPhrase::operator <(const QVariant &other){
    return FieldPhrase(this->data, PhraseData::Less, other);
}

FieldPhrase FieldPhrase::operator >(const QVariant &other){
    qDebug() << "var";
    return FieldPhrase(this->data, PhraseData::Greater, other);
}

FieldPhrase FieldPhrase::operator <=(const QVariant &other){
    return FieldPhrase(this->data, PhraseData::LessEqual, other);
}

FieldPhrase FieldPhrase::operator >=(const QVariant &other){
    return FieldPhrase(this->data, PhraseData::GreaterEqual, other);
}

FieldPhrase FieldPhrase::operator =(const QVariant &other)
{
    return FieldPhrase(this->data, PhraseData::Set, other);
}

FieldPhrase FieldPhrase::isNull(){
    return FieldPhrase(this->data, PhraseData::Null);
}

FieldPhrase FieldPhrase::in(QVariantList list)
{
    return FieldPhrase(this->data, PhraseData::In, list);
}

FieldPhrase FieldPhrase::in(QStringList list)
{
    return FieldPhrase(this->data, PhraseData::In, list);
}

FieldPhrase FieldPhrase::like(QString pattern)
{
    return FieldPhrase(this->data, PhraseData::Like, pattern);
}

QT_END_NAMESPACE


