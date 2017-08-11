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
#include "defines.h"

class QJsonObject;

NUT_BEGIN_NAMESPACE

class TableModel;
struct RelationModel;
class DatabaseModel : public QList<TableModel *>
{
    QString _databaseClassName;
    int _versionMajor, _versionMinor;
    static QMap<QString, DatabaseModel *> _models;

public:
    DatabaseModel(const QString &name = QString::null);
    DatabaseModel(const DatabaseModel &other);

    TableModel *tableByName(QString tableName) const;
    TableModel *tableByClassName(QString className) const;

    RelationModel *relationByClassNames(const QString &masterClassName,
                                        const QString &childClassName);
    RelationModel *relationByTableNames(const QString &masterClassName,
                                        const QString &childClassName);

    bool operator==(const DatabaseModel &other) const;

    static DatabaseModel fromJson(QJsonObject &json);
    QJsonObject toJson() const;

    int versionMajor() const;
    void setVersionMajor(int versionMajor);

    int versionMinor() const;
    void setVersionMinor(int versionMinor);

    bool remove(const QString &tableName);

    static DatabaseModel *modelByName(const QString &name);
};

NUT_END_NAMESPACE

#endif // DATABASEMODEL_H
