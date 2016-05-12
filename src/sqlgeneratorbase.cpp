#include "databasemodel.h"
#include "sqlgeneratorbase_p.h"
#include "table.h"
#include "tablescheema.h"

QT_BEGIN_NAMESPACE

SqlGeneratorBase::SqlGeneratorBase()
{

}

SqlGeneratorBase::~SqlGeneratorBase()
{

}

QString SqlGeneratorBase::saveSql(Table *t, QString tableName)
{
    switch(t->status()){
    case Table::Added:
        return insertCommand(t, tableName);

    case Table::Deleted:
        return deleteCommand(t, tableName);

    case Table::Modified:
        return updateCommand(t, tableName);

    case Table::NewCreated:
    case Table::FeatchedFromDB:
        // disable compiler warning
        return "***";
    }
}

QStringList SqlGeneratorBase::getDiff(DatabaseModel lastModel, DatabaseModel newModel)
{
    QStringList ret;

    QSet<QString> tableNames;
    foreach (TableScheema *table, lastModel)
        tableNames.insert(table->name());

    foreach (TableScheema *table, newModel)
        tableNames.insert(table->name());


    foreach (QString tableName, tableNames) {
        TableScheema *oldTable = lastModel.scheema(tableName);
        TableScheema *newTable = newModel.scheema(tableName);
        ret << getDiff(oldTable, newTable);
    }

    return ret;
}

QString SqlGeneratorBase::getDiff(Field *oldField, Field *newField)
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
        sql.append(getColumnDef(newField));
    }
    return sql;
}

QString SqlGeneratorBase::getDiff(TableScheema *oldTable, TableScheema *newTable)
{
    if(oldTable && newTable)
        if(*oldTable == *newTable)
            return "";

    if(!newTable)
        return "DROP TABLE " + oldTable->name();

    QSet<QString> fieldNames;

    if(oldTable)
        foreach (Field *f, oldTable->fields())
            fieldNames.insert(f->name);

    foreach (Field *f, newTable->fields())
        fieldNames.insert(f->name);

    QStringList columnSql;
    foreach (QString fieldName, fieldNames) {
        Field *newField = newTable->field(fieldName);
        if(oldTable){
            Field *oldField = oldTable->field(fieldName);

            QString buffer =  getDiff(oldField, newField);
            if(!buffer.isNull())
                columnSql << buffer;
        }else{
            columnSql << getColumnDef(newField);
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

QString SqlGeneratorBase::insertCommand(Table *t, QString tableName)
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

QString SqlGeneratorBase::updateCommand(Table *t, QString tableName)
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

QString SqlGeneratorBase::deleteCommand(Table *t, QString tableName)
{
    return QString("DELETE FROM %1 WHERE %2='%3'")
            .arg(tableName)
            .arg(t->primaryKey())
            .arg(t->primaryValue().toString());
}

QString SqlGeneratorBase::deleteTableRows(QString tableName, QString where)
{
    QString sql = "";
    if(where.isEmpty() || where.isNull())
        sql = "DELETE FROM " + tableName;
    else
        sql = "DELETE FROM " + tableName + " WHERE " + where;
    return sql;
}


QT_END_NAMESPACE
