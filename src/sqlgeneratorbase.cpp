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

#include <QDate>
#include <QDateTime>
#include <QTime>
#include <QVariant>

#include "database.h"
#include "databasemodel.h"
#include "sqlgeneratorbase_p.h"
#include "table.h"
#include "tablemodel.h"

QT_BEGIN_NAMESPACE

SqlGeneratorBase::SqlGeneratorBase(Database *parent) : QObject((QObject*)parent)
{
    if(parent)
        _database = parent;
}

SqlGeneratorBase::~SqlGeneratorBase()
{

}

QString SqlGeneratorBase::masterDatabaseName(QString databaseName)
{
    Q_UNUSED(databaseName);
    return "";
}

QString SqlGeneratorBase::saveRecord(Table *t, QString tableName)
{
    switch(t->status()){
    case Table::Added:
        return insertRecord(t, tableName);

    case Table::Deleted:
        return deleteRecord(t, tableName);

    case Table::Modified:
        return updateRecord(t, tableName);

    case Table::NewCreated:
    case Table::FeatchedFromDB:
        // disable compiler warning
        return "";
    }

    return "";
}

QString SqlGeneratorBase::fieldDeclare(FieldModel *field)
{
    return field->name + " " + fieldType(field);
}

QStringList SqlGeneratorBase::diff(DatabaseModel lastModel, DatabaseModel newModel)
{
    QStringList ret;

    QSet<QString> tableNames;
    foreach (TableModel *table, lastModel)
        tableNames.insert(table->name());

    foreach (TableModel *table, newModel)
        tableNames.insert(table->name());


    foreach (QString tableName, tableNames) {
        TableModel *oldTable = lastModel.model(tableName);
        TableModel *newTable = newModel.model(tableName);
        ret << diff(oldTable, newTable);
    }

    return ret;
}

QString SqlGeneratorBase::diff(FieldModel *oldField, FieldModel *newField)
{
    QString sql = "";
    if(oldField && newField)
        if(*oldField == *newField)
            return QString::null;

    if(!newField){
        sql = "DROP COLUMN " + oldField->name;
    }else{
        if(oldField)
            sql = "ALTER COLUMN ";
        else
            sql = "ADD COLUMN ";
        sql.append(fieldDeclare(newField));
    }
    return sql;
}

QString SqlGeneratorBase::diff(TableModel *oldTable, TableModel *newTable)
{
    if(oldTable && newTable)
        if(*oldTable == *newTable)
            return "";

    if(!newTable)
        return "DROP TABLE " + oldTable->name();

    QSet<QString> fieldNames;

    if(oldTable)
        foreach (FieldModel *f, oldTable->fields())
            fieldNames.insert(f->name);

    foreach (FieldModel *f, newTable->fields())
        fieldNames.insert(f->name);

    QStringList columnSql;
    foreach (QString fieldName, fieldNames) {
        FieldModel *newField = newTable->field(fieldName);
        if(oldTable){
            FieldModel *oldField = oldTable->field(fieldName);

            QString buffer =  diff(oldField, newField);
            if(!buffer.isNull())
                columnSql << buffer;
        }else{
            columnSql << fieldDeclare(newField);
        }
    }
    QString sql;
    if(oldTable){
        sql = QString("ALTER TABLE %1 \n%2")
                .arg(newTable->name())
                .arg(columnSql.join(",\n"));
    }else{
        if(!newTable->primaryKey().isNull())
            columnSql << QString("CONSTRAINT pk_%1 PRIMARY KEY (%2)")
                    .arg(newTable->name())
                    .arg(newTable->primaryKey());

        sql = QString("CREATE TABLE %1 \n(%2)")
                .arg(newTable->name())
                .arg(columnSql.join(",\n"));

    }
    return sql;
}

QString SqlGeneratorBase::insertRecord(Table *t, QString tableName)
{
    QString sql = "";
    QString key = t->primaryKey();
    QStringList values;

    foreach (QString f, t->changedProperties())
        if(f != key)
            values.append("'" + t->property(f.toLatin1().data()).toString() + "'");

    sql = QString("INSERT INTO %1 (%2) VALUES (%3)")
            .arg(tableName)
            .arg(t->changedProperties().toList().join(", "))
            .arg(values.join(", "));

    return sql;
}

QString SqlGeneratorBase::updateRecord(Table *t, QString tableName)
{
    QString sql = "";
    QString key = t->primaryKey();
    QStringList values;

    foreach (QString f, t->changedProperties())
        if(f != key)
            values.append(f + "='" + t->property(f.toLatin1().data()).toString() + "'");
    sql = QString("UPDATE %1 SET %2 WHERE %3=%4")
            .arg(tableName)
            .arg(values.join(", "))
            .arg(key)
            .arg(t->primaryValue().toString());

    return sql;
}

QString SqlGeneratorBase::deleteRecord(Table *t, QString tableName)
{
    return QString("DELETE FROM %1 WHERE %2='%3'")
            .arg(tableName)
            .arg(t->primaryKey())
            .arg(t->primaryValue().toString());
}

QString SqlGeneratorBase::deleteRecords(QString tableName, QString where)
{
    QString sql = "";
    if(where.isEmpty() || where.isNull())
        sql = "DELETE FROM " + tableName;
    else
        sql = "DELETE FROM " + tableName + " WHERE " + where;
    return sql;
}

QString SqlGeneratorBase::escapeFieldValue(QVariant &field) const
{
    switch (field.type()) {
    case QVariant::Int:
    case QVariant::Double:
        return field.toString();
        break;

    case QVariant::String:
        return "'" + field.toString() + "'";

    case QVariant::DateTime:
        return "'" + field.toDateTime().toString(Qt::ISODate)  + "'";

    case QVariant::Date:
        return "'" + field.toDate().toString(Qt::ISODate) + "'";

    case QVariant::Time:
        return "'" + field.toTime().toString(Qt::ISODate) + "'";

    case QVariant::StringList:
    case QVariant::List:
        return "['" + field.toStringList().join("', '") + "']";

    case QVariant::Invalid:
        qFatal("Invalud field value");
        return "<Invalid>";

    default:
        return "";
    }
}

QString SqlGeneratorBase::createWhere(QList<WherePhrase> &wheres)
{
    QString whereText = "";
    foreach (WherePhrase p, wheres) {
        if(whereText != "")
            whereText.append(" AND ");
        whereText.append(p.command(this));
    }
    if(whereText != "")
        whereText.prepend(" WHERE ");

    return whereText;
}

QString SqlGeneratorBase::selectCommand(QList<WherePhrase> &wheres, QHash<QString, QString> &orders, QString tableName, QString joinClassName)
{
    QString orderText = "";
    QStringList orderby;

    QString whereText = createWhere(wheres);

    QString tableNameText = tableName;
    if(!joinClassName.isNull()){
        QString joinTableName = _database->tableName(joinClassName);
        RelationModel *rel = _database->model().relationByTableNames(tableName, joinTableName);
        if(rel){
            QString pk = _database->model().model(tableName)->primaryKey();
            tableNameText  = QString("%1 INNER JOIN %2 ON (%1.%3 = %2.%4)")
                    .arg(tableName)
                    .arg(joinTableName)
                    .arg(pk)
                    .arg(rel->localColumn);
            orderby.append(tableName + "." + pk);
        }else{
            qWarning(QString("Relation between table %1 and class %2 (%3) not exists!")
                     .arg(tableName)
                     .arg(joinClassName)
                     .arg(joinTableName.isNull() ? "NULL" : joinTableName)
                     .toLatin1().data());
            joinClassName = QString::null;
        }
    }

    if(orders.count())
        foreach (QString o, orders.keys())
//            orderby.append(o + (orders.value(o) ? " ASC" : " DESC"));
            orderby.append(o + " " + orders.value(o));

    if(orderby.count())
        orderText = " ORDER BY " + orderby.join(", ");

    QString command = "SELECT * FROM "
            + tableNameText
            + whereText
            + orderText;

    for(int i = 0; i < _database->model().count(); i++)
        command = command.replace(_database->model().at(i)->className() + "." , _database->model().at(i)->name() + ".");
    return command;

}

QString SqlGeneratorBase::deleteCommand(QList<WherePhrase> &wheres, QString tableName)
{
    QString command = "DELETE FROM "
            + tableName
            + createWhere(wheres);

    for(int i = 0; i < _database->model().count(); i++)
        command = command.replace(_database->model().at(i)->className() + "." , _database->model().at(i)->name() + ".");
    return command;
}


QT_END_NAMESPACE
