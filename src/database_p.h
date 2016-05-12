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

#ifndef DATABASE_P_H
#define DATABASE_P_H

#include "database.h"
#include "databasemodel.h"
#include "changelogtable.h"

#include <QDebug>


class DatabasePrivate
{
    Database *q_ptr;
    Q_DECLARE_PUBLIC(Database)

public:
    DatabasePrivate(Database *parent);


    bool open();

    bool updateDatabase();
    void createChangeLogs();
    bool storeScheemaInDB();
    DatabaseModel getLastScheema();
    QVariantMap getCurrectScheema();

    QSqlDatabase db;

    QString hostName;
    QString databaseName;
    int port;
    QString userName;
    QString password;
    QString connectionName;
    QString driver;

    QHash<QString, QString> tables;

    SqlGeneratorBase *sqlGenertor;
    DatabaseModel currentModel;

    TableSet<ChangeLogTable> *changeLogs;
};

#endif // DATABASE_P_H
