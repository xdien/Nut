#include "sqlitegenerator.h"
#include "table.h"
#include "tablescheema.h"

SqliteGenerator::SqliteGenerator() : SqlGeneratorBase()
{

}

QString SqliteGenerator::getColumnDef(Field *field)
{
    QString ret = field->name + " ";
    QString dbType;

    switch (field->type) {
    case QVariant::Bool:
        dbType = "int";
        break;
    case QVariant::ByteArray:
        dbType = "blob";
        break;
    case QVariant::Date:
        dbType = "date";
        break;
    case QVariant::DateTime:
        dbType = "datetime";
        break;
    case QVariant::Double:
        dbType = "real";
        break;
    case QVariant::Int:
//        if(field->isAutoIncrement)
//            dbType = "INTEGER PRIMARY KEY";
//        else
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
