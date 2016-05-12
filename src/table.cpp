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

QT_BEGIN_NAMESPACE

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
    static QString ret = QString::null;

    if(ret == QString::null){
        for(int i = 0; i < metaObject()->classInfoCount(); i++){
            QMetaClassInfo ci = metaObject()->classInfo(i);
            QString ciName = ci.name();

            if(ciName.startsWith(__nut_NAME_PERFIX))
                ciName.remove(__nut_NAME_PERFIX);

            if(ciName.contains(" ")){
                QStringList parts = ciName.split(" ");
                QString propName = parts.at(1);
                if(propName == __nut_PRIMARY_KEY)
                    ret = parts.at(0);
            }
        }

        if(ret == QString::null)
            ret = "";
    }

    return ret;
}

QString Table::autoIncrementField() const
{
    static QString ret = QString::null;

    if(ret == QString::null){
        for(int i = 0; i < metaObject()->classInfoCount(); i++){
            QMetaClassInfo ci = metaObject()->classInfo(i);
            QString ciName = ci.name();

            if(ciName.startsWith(__nut_NAME_PERFIX))
                ciName.remove(__nut_NAME_PERFIX);

            if(ciName.contains(" ")){
                QStringList parts = ciName.split(" ");
                QString propName = parts.at(1);
                if(propName == __nut_AUTO_INCREMENT)
                    ret = parts.at(0);
            }
        }

        if(ret == QString::null)
            ret = "";
    }

    return ret;
}

QVariant Table::primaryValue() const
{
    return property(primaryKey().toLatin1().data());
}

void Table::propertyChanged(QString propName)
{
    if(propName == primaryKey())
        return;

//    qDebug() << "Table::propertyChanged" << metaObject()->className() << propName;
    _changedProperties.insert(propName);
    if(_status == FeatchedFromDB)
        _status = Modified;

    if(_status == NewCreated)
        _status = Added;
}

QSet<QString> Table::changedProperties() const
{
    return _changedProperties;
}

bool Table::setParentTable(Table *master)
{
    QString masterClassName = master->metaObject()->className();
    TableScheema *myModel = TableScheema::findByClassName(metaObject()->className());

    foreach (Relation *r, myModel->foregionKeys())
        if(r->className == masterClassName)
        {
            setProperty(QString(r->localColumn).toLatin1().data(), master->primaryValue());
            _changedProperties.insert(r->localColumn);
        }
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

void Table::save(Database *db)
{
    QSqlQuery q = db->exec(db->sqlGenertor()->saveSql(this, db->tableName(metaObject()->className())));
    //if(q.next())
    //    setProperty(primaryKey().toLatin1().data(), q.value(0));

    if(status() == Added)
        setProperty(primaryKey().toLatin1().data(), q.lastInsertId());

    foreach(TableSetBase *ts, tableSets)
        ts->save(db);
    setStatus(FeatchedFromDB);
}

Table::Status Table::status() const
{
    return _status;
}

void Table::setStatus(const Status &status)
{
    _status = status;
}

QT_END_NAMESPACE
