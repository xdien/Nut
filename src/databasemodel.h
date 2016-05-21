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

QT_BEGIN_NAMESPACE

class TableModel;
struct RelationModel;
class QJsonObject;
class DatabaseModel : public QList<TableModel*>
{
    int _versionMajor, _versionMinor;
public:
    DatabaseModel();

    TableModel *model(QString tableName) const;
    TableModel *modelByClass(QString className) const;

    RelationModel *relationByClassNames(QString masterClassName, QString childClassName);
    RelationModel *relationByTableNames(QString masterTableName, QString childTableName);

    bool operator ==(const DatabaseModel &other) const;

    static DatabaseModel fromJson(QJsonObject &json);
    QJsonObject toJson() const;

    int versionMajor() const;
    void setVersionMajor(int versionMajor);

    int versionMinor() const;
    void setVersionMinor(int versionMinor);
};

QT_END_NAMESPACE

#endif // DATABASEMODEL_H
