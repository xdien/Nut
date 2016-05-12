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
#include "tablescheema.h"

#include <QJsonObject>

DatabaseModel::DatabaseModel() : QList<TableScheema*>()
{

}

TableScheema *DatabaseModel::scheema(QString tableName) const
{
    for(int i = 0; i < size(); i++){
        TableScheema *s = at(i);
        if(s->name() == tableName)
            return s;
    }
    return 0;
}

TableScheema *DatabaseModel::scheemaByClass(QString className) const
{
    for(int i = 0; i < size(); i++){
        TableScheema *s = at(i);
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
        TableScheema *mine = at(i);
        TableScheema *others = other.scheema(mine->name());

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

    for(int i = 0; i < size(); i++){
        TableScheema *s = at(i);
        obj.insert(s->name(), s->toJson());
    }

    return obj;
}

Relation *DatabaseModel::relationByClassNames(QString masterClassName, QString childClassName)
{
    TableScheema *childTable = scheemaByClass(childClassName);

    if(!childTable)
        return 0;

    foreach (Relation *rel, childTable->foregionKeys())
        if(rel->className == masterClassName)
            return rel;

    return 0;
}

Relation *DatabaseModel::relationByTableNames(QString masterTableName, QString childTableName)
{
    TableScheema *childTable = scheema(childTableName);

    if(!childTable)
        return 0;

    foreach (Relation *rel, childTable->foregionKeys())
        if(rel->table->name() == masterTableName)
            return rel;

    return 0;
}

DatabaseModel DatabaseModel::fromJson(QJsonObject &json)
{
    DatabaseModel model;
    foreach (QString key, json.keys()) {
        TableScheema *sch = new TableScheema(json.value(key).toObject(), key);
        model.append(sch);
    }
    return model;
}
