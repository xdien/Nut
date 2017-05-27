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

NUT_BEGIN_NAMESPACE

PhraseData::PhraseData(const char *className, const char *s){
    text = QString(s);// QString(className) + "." + s;
    type = Field;
    operatorCond = NotAssign;
//    qDebug() << "(" << this << ")" << "Data type 0";
}

PhraseData::PhraseData(PhraseData *l, PhraseData::Condition o) : left(l){
    operatorCond = o;
    type = WithoutOperand;
//    qDebug() << "(" << this << ")" << "Data type 1";
}

PhraseData::PhraseData(PhraseData *l, PhraseData::Condition o, const PhraseData *r) : left(l), right(r){
    operatorCond = o;
    type = WithOther;
//    qDebug() << "(" << this << ")" << "Data type 2";
}

PhraseData::PhraseData(PhraseData *l, PhraseData::Condition o, QVariant r) : left(l), operand(r){
    operatorCond = o;
    type = WithVariant;
//    qDebug() << "(" << this << ")" << "Data type 1";
}

PhraseData::~PhraseData(){
//    qDebug() << "(" << this << ")" << "Data Deleting..." << type;
    if(type == WithOther){
//        qDebug() << "   - Other" << left << right;
        delete left;
        delete right;
    }
    if(type == WithVariant){
//        qDebug() << "   - Variant" << left;
        if(left)
            delete left;
    }
}

PhraseData *WherePhrase::data() const
{
    return _data;
}

WherePhrase::WherePhrase(const char *className, const char *s)
{
//    qDebug() << "(" << this << ")" << "class ctor" << className << s;
    _data = new PhraseData(className, s);
}

WherePhrase::WherePhrase(const WherePhrase &l)
{
    _data = l._data;
    //    l._data = 0;
//    qDebug() << "(" << this << ")" << "Copy ctor, from" <<  _data << (&l);
    _dataPointer = QSharedPointer<PhraseData>(l._dataPointer);
}

WherePhrase::WherePhrase(WherePhrase *l)
{
    _data = l->_data;

//    qDebug() << "(" << this << ")" << "From pointer" << _data;
//    _dataPointer = QSharedPointer<PhraseData>(_data);
    l->_data = 0;
    l->_dataPointer.reset(0);
}

WherePhrase::WherePhrase(WherePhrase *l, PhraseData::Condition o)
{
    _data = new PhraseData(l->_data, o);
//    _dataPointer = QSharedPointer<PhraseData>(_data);
    l->_data = 0;
//    qDebug() << "(" << this << ")" << "From cond, " << _data << o;
    l->_dataPointer.reset(0);
}


WherePhrase::WherePhrase(WherePhrase *l, PhraseData::Condition o, WherePhrase *r)
{
    _data = new PhraseData(l->_data, o, r->_data);
//    _dataPointer = QSharedPointer<PhraseData>(_data);
    l->_data = 0;
    r->_data = 0;

//    qDebug() << "(" << this << ")" <<  "From two pointer" << _data;
    l->_dataPointer.reset(0);
    r->_dataPointer.reset(0);
}

WherePhrase::WherePhrase(WherePhrase *l, PhraseData::Condition o, QVariant r)
{
    _data = new PhraseData(l->_data, o, r);
//    _dataPointer = QSharedPointer<PhraseData>(_data);
    l->_data = 0;

//    qDebug() << "(" << this << ")" << "From variant," << _data << l << r;
    l->_dataPointer.reset(0);
}

WherePhrase::~WherePhrase()
{
//    qDebug() << "(" << this << ")" << "Dtor" << _data << _dataPointer.data();
    //    if(_data){
    //        delete _data;
    //        qDebug() << "deleted";
    //    }
}

WherePhrase WherePhrase::operator ==(const WherePhrase &other)
{
    return WherePhrase(this, PhraseData::Equal, (WherePhrase*)&other);
}

WherePhrase WherePhrase::operator !=(const WherePhrase &other)
{
    return WherePhrase(this, PhraseData::NotEqual, (WherePhrase*)&other);
}

WherePhrase WherePhrase::operator <(const WherePhrase &other)
{
    return WherePhrase(this, PhraseData::Less, (WherePhrase*)&other);
}

WherePhrase WherePhrase::operator >(const WherePhrase &other)
{
    return WherePhrase(this, PhraseData::Greater, (WherePhrase*)&other);
}

WherePhrase WherePhrase::operator <=(const WherePhrase &other)
{
    return WherePhrase(this, PhraseData::LessEqual, (WherePhrase*)&other);
}

WherePhrase WherePhrase::operator >=(const WherePhrase &other)
{
    return WherePhrase(this, PhraseData::GreaterEqual, (WherePhrase*)&other);
}

WherePhrase WherePhrase::operator =(const WherePhrase &other)
{
    return WherePhrase(this, PhraseData::Set, (WherePhrase*)&other);
}

WherePhrase WherePhrase::operator +(const WherePhrase &other)
{
    return WherePhrase(this, PhraseData::Add, (WherePhrase*)&other);
}

WherePhrase WherePhrase::operator -(const WherePhrase &other)
{
    return WherePhrase(this, PhraseData::Minus, (WherePhrase*)&other);
}

WherePhrase WherePhrase::operator *(const WherePhrase &other)
{
    return WherePhrase(this, PhraseData::Multiple, (WherePhrase*)&other);
}

WherePhrase WherePhrase::operator /(const WherePhrase &other)
{
    return WherePhrase(this, PhraseData::Divide, (WherePhrase*)&other);
}

WherePhrase WherePhrase::operator &&(const WherePhrase &other)
{
    return WherePhrase(this, PhraseData::And, (WherePhrase*)&other);
}

WherePhrase WherePhrase::operator ||(const WherePhrase &other)
{
    return WherePhrase(this, PhraseData::Or, (WherePhrase*)&other);
}

WherePhrase WherePhrase::operator &(const WherePhrase &other)
{
    return WherePhrase(this, PhraseData::Append, (WherePhrase*)&other);
}

WherePhrase WherePhrase::operator ==(const QVariant &other)
{
    return WherePhrase(this, PhraseData::Equal, other);
}

WherePhrase WherePhrase::operator !=(const QVariant &other)
{
    return WherePhrase(this, PhraseData::NotEqual, other);
}

WherePhrase WherePhrase::operator <(const QVariant &other)
{
    return WherePhrase(this, PhraseData::Less, other);
}

WherePhrase WherePhrase::operator >(const QVariant &other)
{
    return WherePhrase(this, PhraseData::Greater, other);
}

WherePhrase WherePhrase::operator <=(const QVariant &other)
{
    return WherePhrase(this, PhraseData::LessEqual, other);
}

WherePhrase WherePhrase::operator >=(const QVariant &other)
{
    return WherePhrase(this, PhraseData::GreaterEqual, other);
}

NUT_END_NAMESPACE


