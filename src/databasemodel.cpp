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

#include "databasemodel.h"
#include "tablemodel.h"

#include <QJsonObject>

NUT_BEGIN_NAMESPACE

DatabaseModel::DatabaseModel() : QList<TableModel*>(), _versionMajor(0), _versionMinor(0)
{

}

DatabaseModel::DatabaseModel(const DatabaseModel &other) : QList<TableModel*>(other), _versionMajor(0), _versionMinor(0)
{

}

TableModel *DatabaseModel::model(QString tableName) const
{
    for(int i = 0; i < size(); i++){
        TableModel *s = at(i);

        if(s->name() == tableName)
            return s;
    }
    return 0;
}

TableModel *DatabaseModel::modelByClass(QString className) const
{
    for(int i = 0; i < size(); i++){
        TableModel *s = at(i);
        if(s->className() == className)
            return s;
    }
    return 0;
}

bool DatabaseModel::operator ==(const DatabaseModel &other) const
{
    if(size() != other.size())
        return false;

    for(int i = 0; i < size(); i++){
        TableModel *mine = at(i);
        TableModel *others = other.model(mine->name());

        if(!others)
            return false;

        if(*mine != *others)
            return false;
    }

    return true;
}

QJsonObject DatabaseModel::toJson() const
{
    QJsonObject obj;

//    obj.insert(QT_STRINGIFY(versionMajor), QJsonValue(_versionMajor));
//    obj.insert(QT_STRINGIFY(versionMinor), QJsonValue(_versionMinor));

    for(int i = 0; i < size(); i++){
        TableModel *s = at(i);
        obj.insert(s->name(), s->toJson());
    }

    return obj;
}

RelationModel *DatabaseModel::relationByClassNames(QString masterClassName, QString childClassName)
{
    TableModel *childTable = modelByClass(childClassName);

    if(!childTable)
        return 0;

    foreach (RelationModel *rel, childTable->foregionKeys())
        if(rel->className == masterClassName)
            return rel;

    return 0;
}

RelationModel *DatabaseModel::relationByTableNames(QString masterTableName, QString childTableName)
{
    TableModel *childTable = model(childTableName);

    if(!childTable)
        return 0;

    foreach (RelationModel *rel, childTable->foregionKeys())
        if(rel->table->name() == masterTableName)
            return rel;

    return 0;
}

DatabaseModel DatabaseModel::fromJson(QJsonObject &json)
{
    DatabaseModel model;

//    model.setVersionMajor(json.value(QT_STRINGIFY(versionMajor)).toInt());
//    model.setVersionMinor(json.value(QT_STRINGIFY(versionMinor)).toInt());

    foreach (QString key, json.keys()) {
        if(!json.value(key).isObject())
            continue;

        TableModel *sch = new TableModel(json.value(key).toObject(), key);
        model.append(sch);
    }
    return model;
}

int DatabaseModel::versionMajor() const
{
    return _versionMajor;
}

void DatabaseModel::setVersionMajor(int versionMajor)
{
    _versionMajor = versionMajor;
}

int DatabaseModel::versionMinor() const
{
    return _versionMinor;
}

void DatabaseModel::setVersionMinor(int versionMinor)
{
    _versionMinor = versionMinor;
}

bool DatabaseModel::remove(QString tableName)
{
    for(int i = 0; i < size(); i++){
        TableModel *s = at(i);
        if(s->name() == tableName){
            removeAt(i);
            return true;
        }
    }
    return false;
}

NUT_END_NAMESPACE
