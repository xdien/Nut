#include "postgresqlgenerator.h"
#include "table.h"
#include "tablescheema.h"

QT_BEGIN_NAMESPACE

PostgreSqlGenerator::PostgreSqlGenerator() : SqlGeneratorBase ()
{

}

QString PostgreSqlGenerator::getColumnDef(Field *field)
{
    QString ret = field->name + " ";
    QString dbType;

    switch (field->type) {
    case QVariant::Bool:
        dbType = "boolean";
        break;
    case QVariant::ByteArray:
        dbType = "bytea";
        break;
    case QVariant::Date:
        dbType = "date";
        break;
    case QVariant::DateTime:
        dbType = "timestamp";
        break;
    case QVariant::Double:
        dbType = "real";
        break;
    case QVariant::Int:
        if(field->isAutoIncrement)
            dbType = "serial";
        else
            dbType = "integer";
        break;
    case QVariant::String:
        if(field->length)
            dbType = QString("varchar(%1)").arg(field->length);
        else
            dbType = "text";
        break;
    case QVariant::Time:
        dbType = "time";
        break;
    default:
        dbType = "";
    }
    ret.append(dbType);
    return ret;
}

//QString PostgreSqlGenerator::saveSql(Table *t, QString tableName)
//{
//    switch(t->status()){
//    case Table::Added:
//        return insertCommand(t, tableName) + " RETURNING " + t->primaryKey();

//    default:
//        return SqlGeneratorBase::saveSql(t, tableName);
//    }
//}

QString PostgreSqlGenerator::deleteTableRows(QString tableName, QString where)
{
    return SqlGeneratorBase::deleteTableRows(tableName, where) + " RETURNING *";
}

QT_END_NAMESPACE
