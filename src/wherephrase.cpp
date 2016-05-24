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


WherePhrase::WherePhrase(const char *className, const char *s) : willDeleteData(false)
{
    data = new PhraseData(className, s);
    text = QString(className) + "." + s;
}

WherePhrase::WherePhrase(PhraseData *l) : willDeleteData(false)
{
    data = l;
}

WherePhrase::WherePhrase(PhraseData *l, PhraseData::Condition o) : willDeleteData(false)
{
    data = new PhraseData(l, o);
}


WherePhrase::WherePhrase(PhraseData *l, PhraseData::Condition o, PhraseData *r) : willDeleteData(false)
{
    data = new PhraseData(l, o, r);
}

WherePhrase::WherePhrase(PhraseData *l, PhraseData::Condition o, QVariant r) : willDeleteData(false)
{
    data = new PhraseData(l, o, r);
}

WherePhrase::~WherePhrase()
{
//    if(willDeleteData)
//        delete data;
}


QString WherePhrase::command(SqlGeneratorBase *generator)
{
    willDeleteData = true;
    return data->command(generator);
}

void WherePhrase::deleteData(PhraseData *d)
{
    deleteData(d);
    if(d->type == PhraseData::WithOther){
        delete d->left;
        delete d->right;
    }
    if(d->type == PhraseData::WithVariant)
        delete d->left;
}

WherePhrase WherePhrase::operator ==(const WherePhrase &other){
    return WherePhrase(this->data, PhraseData::Equal, other.data);
}

WherePhrase WherePhrase::operator !=(const WherePhrase &other){
    return WherePhrase(this->data, PhraseData::NotEqual, other.data);
}

WherePhrase WherePhrase::operator <(const WherePhrase &other){
    return WherePhrase(this->data, PhraseData::Less, other.data);
}

WherePhrase WherePhrase::operator >(const WherePhrase &other){
    return WherePhrase(this->data, PhraseData::Greater, other.data);
}

WherePhrase WherePhrase::operator <=(const WherePhrase &other){
    return WherePhrase(this->data, PhraseData::LessEqual, other.data);
}

WherePhrase WherePhrase::operator >=(const WherePhrase &other){
    return WherePhrase(this->data, PhraseData::GreaterEqual, other.data);
}

WherePhrase WherePhrase::operator =(const WherePhrase &other)
{
    return WherePhrase(this->data, PhraseData::Set, other.data);
}

WherePhrase WherePhrase::operator +(const WherePhrase &other){
    return WherePhrase(this->data, PhraseData::Add, other.data);
}

WherePhrase WherePhrase::operator -(const WherePhrase &other){
    return WherePhrase(this->data, PhraseData::Minus, other.data);
}

WherePhrase WherePhrase::operator *(const WherePhrase &other){
    return WherePhrase(this->data, PhraseData::Multiple, other.data);
}

WherePhrase WherePhrase::operator /(const WherePhrase &other){
    return WherePhrase(this->data, PhraseData::Divide, other.data);
}

WherePhrase WherePhrase::operator &&(const WherePhrase &other){
    return WherePhrase(this->data, PhraseData::And, other.data);
}

WherePhrase WherePhrase::operator ||(const WherePhrase &other){
    return WherePhrase(this->data, PhraseData::Or, other.data);
}

WherePhrase WherePhrase::operator &(const WherePhrase &other)
{
    return WherePhrase(this->data, PhraseData::Append, other.data);
}

WherePhrase FieldPhrase::operator !(){
    if(data->operatorCond < 20)
        data->operatorCond = (PhraseData::Condition)((data->operatorCond + 10) % 20);
    else
        qFatal("Operator ! can not aplied to non condition statements");

    return WherePhrase(data);
}

WherePhrase WherePhrase::operator ==(const QVariant &other){
    return WherePhrase(this->data, PhraseData::Equal, other);
}

WherePhrase WherePhrase::operator !=(const QVariant &other){
    return WherePhrase(this->data, PhraseData::NotEqual, other);
}

WherePhrase WherePhrase::operator <(const QVariant &other){
    return WherePhrase(this->data, PhraseData::Less, other);
}

WherePhrase WherePhrase::operator >(const QVariant &other){
    qDebug() << "var";
    return WherePhrase(this->data, PhraseData::Greater, other);
}

WherePhrase WherePhrase::operator <=(const QVariant &other){
    return WherePhrase(this->data, PhraseData::LessEqual, other);
}

WherePhrase WherePhrase::operator >=(const QVariant &other){
    return WherePhrase(this->data, PhraseData::GreaterEqual, other);
}

WherePhrase FieldPhrase::operator =(const QVariant &other)
{
    return WherePhrase(this->data, PhraseData::Set, other);
}

FieldPhrase::FieldPhrase(const char *className, const char *s) : WherePhrase(className, s)
{
    data = new PhraseData(className, s);
    text = QString(className) + "." + s;
}

WherePhrase FieldPhrase::operator &(const QVariant &other)
{
    Q_UNUSED(other);
    qFatal("The operator & can not applied for two fields");
}

WherePhrase FieldPhrase::isNull(){
    return WherePhrase(this->data, PhraseData::Null);
}

WherePhrase FieldPhrase::in(QVariantList list)
{
    return WherePhrase(this->data, PhraseData::In, list);
}

WherePhrase FieldPhrase::in(QStringList list)
{
    return WherePhrase(this->data, PhraseData::In, list);
}

WherePhrase FieldPhrase::like(QString pattern)
{
    return WherePhrase(this->data, PhraseData::Like, pattern);
}

QT_END_NAMESPACE


