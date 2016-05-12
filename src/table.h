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

#ifndef TABLE_H
#define TABLE_H

#include <QtCore/QObject>
#include <QtCore/qglobal.h>
#include <QtCore/QSet>

#include "tablescheema.h"
#include "defines.h"
//#include "tableset.h"

QT_BEGIN_NAMESPACE

class Database;
class TableSetBase;
class NUT_EXPORT Table : public QObject
{
    Q_OBJECT

public:
    explicit Table(QObject *tableSet = 0);

    enum Status{
        NewCreated,
        FeatchedFromDB,
        Added,
        Modified,
        Deleted
    };

    void add(TableSetBase *);
    void save(Database *db);

    QString primaryKey() const;
    QString autoIncrementField() const;
    QVariant primaryValue() const;
    Status status() const;
    void setStatus(const Status &status);

    TableSetBase *tableSet() const;
    void setTableSet(TableSetBase *tableSet);

    QSet<QString> changedProperties() const;

    bool setParentTable(Table *master);
signals:

public slots:

protected:
    void propertyChanged(QString propName);

private:
    Status _status;
    QSet<QString> _changedProperties;
    TableSetBase *_tableSet;

    QSet<TableSetBase*> tableSets;

//    template<class T>
//    friend class TableSet;
    template<class T>
    friend class Query;
//    friend class Database;
};

QT_END_NAMESPACE

#include "tableset.cpp"
#endif // TABLE_H
