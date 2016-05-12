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

#ifndef DATABASEMODEL_H
#define DATABASEMODEL_H

#include <QtCore/QList>

class TableScheema;
struct Relation;
class QJsonObject;
class DatabaseModel : public QList<TableScheema*>
{
public:
    DatabaseModel();

    TableScheema *scheema(QString tableName) const;
    TableScheema *scheemaByClass(QString className) const;

    Relation *relationByClassNames(QString masterClassName, QString childClassName);
    Relation *relationByTableNames(QString masterTableName, QString childTableName);

    bool operator ==(const DatabaseModel &other) const;

    static DatabaseModel fromJson(QJsonObject &json);
    QJsonObject toJson() const;

};

#endif // DATABASEMODEL_H
