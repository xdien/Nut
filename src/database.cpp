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

#include <QtCore/QMetaProperty>
#include <QtCore/QDebug>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlResult>

#include "database.h"
#include "table.h"
#include "tableset.h"
#include "database_p.h"
#include "defines_p.h"
#include "tablemodel.h"
#include "postgresqlgenerator.h"
#include "mysqlgenerator.h"
#include "sqlitegenerator.h"
#include "sqlservergenerator.h"
#include "query.h"

#include <iostream>
#include <cstdarg>

QT_BEGIN_NAMESPACE

DatabasePrivate::DatabasePrivate(Database *parent) : q_ptr(parent)
{
}

bool DatabasePrivate::open()
{
    getCurrectScheema();

    db = QSqlDatabase::addDatabase(driver, connectionName);
    db.setHostName(hostName);
    db.setDatabaseName(databaseName);
    db.setUserName(userName);
    db.setPassword(password);
    bool ok = db.open();

    if(!ok){
        qWarning("Could not connect to database");
        qWarning(db.lastError().text().toLocal8Bit().data());

        if(db.lastError().text().contains("database \"" + databaseName + "\" does not exist")
                || db.lastError().text().contains("Cannot open database")){
            db.setDatabaseName(sqlGenertor->masterDatabaseName(databaseName));
            ok = db.open();
            qInfo("Creating database");
            if(ok){
                db.exec("CREATE DATABASE " + databaseName);
                db.close();

                if(db.lastError().type() != QSqlError::NoError)
                    qWarning(db.lastError().text().prepend("Creating database error: ").toLatin1().data());

                return open();
            }else{
                qWarning(db.lastError().text().toLatin1().data());
            }
        }
        return false;
    }

    return updateDatabase();
}

bool DatabasePrivate::updateDatabase()
{
    DatabaseModel last = getLastScheema();
    DatabaseModel current = currentModel;

    if(last == current){
        qInfo("Databse is up-to-date");
        return true;
    }

    if(!last.count())
        qInfo("Databse is new");
    else
        qInfo("Databse is changed");

    QStringList sql = sqlGenertor->diff(last, current);
    db.transaction();
    foreach (QString s, sql){
        qDebug() << "going to exec " << s;
        db.exec(s);

        if(!db.lastError().type() == QSqlError::NoError)
            qWarning(db.lastError().text().toLatin1().data());
    }
    bool ok = db.commit();

    if(ok){
        storeScheemaInDB();
    }else{
        qWarning("Unable update database");
        qWarning(db.lastError().text().toLatin1().data());
    }

    return ok;
}

QVariantMap DatabasePrivate::getCurrectScheema()
{
    Q_Q(Database);
    tables.clear();

    //TODO: change logs must not be in model
    int changeLogTypeId = qRegisterMetaType<ChangeLogTable*>();
    currentModel.append(new TableModel(changeLogTypeId, "__change_logs"));
    tables.insert("ChangeLogTable", "__change_logs");

    for(int i = 0; i < q->metaObject()->classInfoCount(); i++){
        QMetaClassInfo ci = q->metaObject()->classInfo(i);
        QString ciName = QString(ci.name()).replace(__nut_NAME_PERFIX, "");
        if(ciName.startsWith(__nut_TABLE))
            tables.insert(QString(ci.name()).replace(__nut_NAME_PERFIX, "").split(" ").at(1), ci.value());

        if(ciName == __nut_DB_VERSION){
            QStringList version = QString(ci.value()).split('.');
            bool ok = false;
            if(version.length() == 1){
                currentModel.setVersionMajor(version.at(0).toInt(&ok));
            } else if(version.length() == 2){
                currentModel.setVersionMajor(version.at(0).toInt(&ok));
                currentModel.setVersionMinor(version.at(1).toInt(&ok));
            }

            if(!ok)
                qFatal("NUT_DB_VERSION macro accept version in format 'x' or 'x.y' only, and x[,y] must be integer values\n");
        }
    }

    QVariantMap databaseVariant;
    for(int i = 1; i < q->metaObject()->propertyCount(); i++){
        QMetaProperty tableProperty = q->metaObject()->property(i);
        uint typeId = QMetaType::type(tableProperty.typeName());

        if(tables.values().contains(tableProperty.name()) && typeId >= QVariant::UserType){
            TableModel *sch = new TableModel(typeId, tableProperty.name());
            currentModel.append(sch);
        }
    }

    foreach (TableModel *sch, currentModel)
        foreach (RelationModel *fk, sch->foregionKeys())
            fk->table = currentModel.modelByClass(fk->className);


    return databaseVariant;
}

DatabaseModel DatabasePrivate::getLastScheema()
{
    ChangeLogTable *u = changeLogs->createQuery()->orderBy("id", "desc")->first();

    DatabaseModel ret;

    if(u){
        QJsonObject json = QJsonDocument::fromJson(QByteArray(u->data().toLocal8Bit().data())).object();

        foreach (QString key, json.keys()) {
            TableModel *sch = new TableModel(json.value(key).toObject(), key);
            ret.append(sch);
        }

        u->deleteLater();
    }
    return ret;
}

bool DatabasePrivate::storeScheemaInDB()
{
    Q_Q(Database);
    ChangeLogTable *changeLog = new ChangeLogTable();
    changeLog->setData(QJsonDocument(currentModel.toJson()).toJson());
    changeLog->setVersionMajor(currentModel.versionMajor());
    changeLog->setVersionMinor(currentModel.versionMinor());
    changeLogs->append(changeLog);
    q->saveChanges();
    changeLog->deleteLater();

    return true;
//    QSqlQuery q(db);
//    q.prepare("insert into __change_logs (data) values (:data)");
//    q.bindValue(":data", QString(QJsonDocument(currentModel.toJson()).toJson()));
//    bool ret = q.exec();
//    if(q.lastError().type() != QSqlError::NoError)
//        qWarning(QString("storeScheemaInDB" + q.lastError().text()).toLatin1().data());
//    return ret;
}

void DatabasePrivate::createChangeLogs()
{
    QString diff = sqlGenertor->diff(0, currentModel.model("__change_logs"));

    db.exec(diff);
}

Database::Database(QObject *parent) : QObject(parent), d_ptr(new DatabasePrivate(this))
{
    Q_D(Database);
    d->changeLogs = new TableSet<ChangeLogTable>(this);
}

QString Database::databaseName() const
{
    Q_D(const Database);
    return d->databaseName;
}

QString Database::hostName() const
{
    Q_D(const Database);
    return d->hostName;
}

int Database::port() const
{
    Q_D(const Database);
    return d->port;
}

QString Database::userName() const
{
    Q_D(const Database);
    return d->userName;
}

QString Database::password() const
{
    Q_D(const Database);
    return d->password;
}

QString Database::connectionName() const
{
    Q_D(const Database);
    return d->connectionName;
}

QString Database::driver() const
{
    Q_D(const Database);
    return d->driver;
}

DatabaseModel Database::model() const
{
    Q_D(const Database);
    return d->currentModel;
}

QString Database::tableName(QString className)
{
    Q_D(Database);
    return d->tables[className];
}

void Database::setDatabaseName(QString databaseName)
{
    Q_D(Database);
    d->databaseName = databaseName;
}

void Database::setHostName(QString hostName)
{
    Q_D(Database);
    d->hostName = hostName;
}

void Database::setPort(int port)
{
    Q_D(Database);
    d->port = port;
}

void Database::setUserName(QString username)
{
    Q_D(Database);
    d->userName = username;
}

void Database::setPassword(QString password)
{
    Q_D(Database);
    d->password = password;
}

void Database::setConnectionName(QString connectionName)
{
    Q_D(Database);
    d->connectionName = connectionName;
}

void Database::setDriver(QString driver)
{
    Q_D(Database);
    d->driver = driver;
}

SqlGeneratorBase *Database::sqlGenertor() const
{
    Q_D(const Database);
    return d->sqlGenertor;
}

bool Database::open()
{
    Q_D(Database);

    if(d->driver == "QPSQL" || d->driver == "QPSQL7")
        d->sqlGenertor = new PostgreSqlGenerator;
    else if (d->driver == "QMYSQL" || d->driver == "QMYSQL3")
        d->sqlGenertor = new MySqlGenerator;
    else if (d->driver == "QSQLITE" || d->driver == "QSQLITE3")
        d->sqlGenertor = new SqliteGenerator;
    else if(d->driver == "QODBC" || d->driver == "QODBC3"){
        QString driverName = QString::null;
        QStringList parts = d->databaseName.toLower().split(';');
        foreach (QString p, parts)
            if(p.trimmed().startsWith("driver="))
                driverName = p.split('=').at(1);

        if(driverName == "{sql server}")
            d->sqlGenertor = new SqlServerGenerator;
    }

    if(!d->sqlGenertor){
        qWarning(QString("Sql generator for driver " + driver() + " not found").toLatin1().data());
        return false;
    }else{
        return d->open();
    }
}

QSqlQuery Database::exec(QString sql)
{
    Q_D(Database);
    QSqlQuery q = d->db.exec(sql);
    if(d->db.lastError().type() != QSqlError::NoError)
        qWarning(d->db.lastError().text().toLatin1().data());
    return q;
}

void Database::add(TableSetBase *t)
{
    tableSets.insert(t);
}

void Database::saveChanges()
{
    foreach(TableSetBase *ts, tableSets)
        ts->save(this);
}

void Database::cleanUp()
{
    foreach(TableSetBase *ts, tableSets)
        ts->clearChilds();
}

QT_END_NAMESPACE
