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

#include <QMetaMethod>
#include <QVariant>
#include "table.h"
#include "database.h"
#include "sqlgeneratorbase_p.h"

NUT_BEGIN_NAMESPACE

Table::Table(QObject *parent) : QObject(parent)
{
    setStatus(NewCreated);
}

void Table::add(TableSetBase *t)
{
    this->tableSets.insert(t);
}


QString Table::primaryKey() const
{
//    static QString ret = QString::null;

//    if(ret == QString::null){
//        for(int i = 0; i < metaObject()->classInfoCount(); i++){
//            QMetaClassInfo ci = metaObject()->classInfo(i);
//            QString ciName = ci.name();

//            if(ciName.startsWith(__nut_NAME_PERFIX))
//                ciName.remove(__nut_NAME_PERFIX);

//            if(ciName.contains(" ")){
//                QStringList parts = ciName.split(" ");
//                QString propName = parts.at(1);
//                if(propName == __nut_PRIMARY_KEY)
//                    ret = parts.at(0);
//            }
//        }

//        if(ret == QString::null)
//            ret = "";
//    }

//    return ret;
    return TableModel::findByClassName(metaObject()->className())->primaryKey();
}

bool Table::isPrimaryKeyAutoIncrement() const
{
    return TableModel::findByClassName(metaObject()->className())->field(primaryKey())->isAutoIncrement;
}


QVariant Table::primaryValue() const
{
    return property(primaryKey().toLatin1().data());
}

void Table::propertyChanged(QString propName)
{
    if(propName == primaryKey())
        return;

    _changedProperties.insert(propName);
    if(_status == FeatchedFromDB)
        _status = Modified;

    if(_status == NewCreated)
        _status = Added;
}

void Table::clear()
{
    _changedProperties.clear();
}

QSet<QString> Table::changedProperties() const
{
    return _changedProperties;
}

bool Table::setParentTable(Table *master)
{
    QString masterClassName = master->metaObject()->className();
    TableModel *myModel = TableModel::findByClassName(metaObject()->className());

    foreach (RelationModel *r, myModel->foregionKeys())
        if(r->className == masterClassName)
        {
            setProperty(QString(r->localColumn).toLatin1().data(), master->primaryValue());
            _changedProperties.insert(r->localColumn);
            return true;
        }

    return false;
}

TableSetBase *Table::tableSet() const
{
    return _tableSet;
}

void Table::setTableSet(TableSetBase *parent)
{
    _tableSet = parent;
    _tableSet->add(this);
}

int Table::save(Database *db)
{
    QSqlQuery q = db->exec(db->sqlGenertor()->saveRecord(this, db->tableName(metaObject()->className())));

    if(status() == Added && isPrimaryKeyAutoIncrement())
        setProperty(primaryKey().toLatin1().data(), q.lastInsertId());

    foreach(TableSetBase *ts, tableSets)
        ts->save(db);
    setStatus(FeatchedFromDB);

    return q.numRowsAffected();
}

Table::Status Table::status() const
{
    return _status;
}

void Table::setStatus(const Status &status)
{
    _status = status;
}

NUT_END_NAMESPACE
