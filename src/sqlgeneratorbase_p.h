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

#ifndef SQLGENERATORBASE_H
#define SQLGENERATORBASE_H

#include <QtCore/qglobal.h>
#include <QtCore/QObject>
#include <QtCore/QStringList>
#include "wherephrase.h"

NUT_BEGIN_NAMESPACE

class Table;
struct FieldModel;
class DatabaseModel;
class TableModel;
class Database;
//struct PhraseData;
//class WherePhrase;
class SqlGeneratorBase : public QObject
{
//    Q_OBJECT

    Database *_database;
public:
    enum CommandType{
        Select,
        Insert,
        Update,
        Delete
    };
    enum AgregateType{
        SelectAll,
        Count,
        Min,
        Max,
        Average,
        SignleField
    };

    SqlGeneratorBase(Database *parent);
    virtual ~SqlGeneratorBase();

    virtual QString masterDatabaseName(QString databaseName);

    virtual QString fieldType(FieldModel *field) = 0;
    virtual QString fieldDeclare(FieldModel *field);

    virtual QStringList diff(DatabaseModel lastModel, DatabaseModel newModel);
    virtual QString diff(FieldModel *oldField, FieldModel *newField);
    virtual QString diff(TableModel *oldTable, TableModel *newTable);

    virtual QString saveRecord(Table *t, QString tableName);
    virtual QString insertRecord(Table *t, QString tableName);
    virtual QString updateRecord(Table *t, QString tableName);
    virtual QString deleteRecord(Table *t, QString tableName);


    virtual QString deleteRecords(QString tableName, QString where);

    virtual QString selectCommand(AgregateType t, QString agregateArg,
                                  QList<WherePhrase> &wheres, QList<WherePhrase> &orders,
                                  QString tableName, QString joinClassName);

    virtual QString selectCommand(QString selectPhrase, QString agregateArg,
                                  QList<WherePhrase> &wheres, QList<WherePhrase> &orders,
                                  QString tableName, QString joinClassName);

    virtual QString selectCommand(QList<WherePhrase> &wheres, QHash<QString, QString>  &orders,
                                  QString tableName, QString joinClassName);
    virtual QString selectCommand(QString selectPhrase,
                                  QList<WherePhrase> &wheres, QHash<QString, QString>  &orders,
                                  QString tableName, QString joinClassName);

    virtual QString deleteCommand(QList<WherePhrase> &wheres, QString tableName);

    virtual QString updateCommand(WherePhrase &phrase, QList<WherePhrase> &wheres, QString tableName);

    virtual QString escapeValue(const QVariant &v) const;
    virtual QVariant readValue(const QVariant::Type &type, const QVariant &dbValue);
    virtual QString phrase(const PhraseData *d) const;
    virtual QString phraseUpdate(const PhraseData *d) const;
    virtual QString operatorString(const PhraseData::Condition &cond) const;

private:
    QString agregateText(const AgregateType &t, const QString &arg = QString::null) const;
    QString fromTableText(const QString &tableName, QString &joinClassName, QString &orderBy) const;
    QString createWhere(QList<WherePhrase> &wheres);
    QString phraseOrder(const PhraseData *d) const;
};

NUT_END_NAMESPACE

#endif // SQLGENERATORBASE_H
