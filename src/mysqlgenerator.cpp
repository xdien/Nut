#include "mysqlgenerator.h"
#include "tablescheema.h"

QT_BEGIN_NAMESPACE

MySqlGenerator::MySqlGenerator() : SqlGeneratorBase()
{

}

QString MySqlGenerator::getColumnDef(Field *field)
{
    QString ret = field->name + " ";
    QString dbType;

    switch (field->type) {
    case QVariant::Bool:
        dbType = "boolean";
        break;
    case QVariant::ByteArray:
        dbType = "blob";
        break;
    case QVariant::Double:
        dbType = "real";
        break;
    case QVariant::Int:
        dbType = "int(4)";
        if(field->isAutoIncrement)
            dbType += " auto_increment";

        break;
    case QVariant::String:
        if(field->length)
            dbType = QString("varchar(%1)").arg(field->length);
        else
            dbType = "text";
        break;
    case QVariant::DateTime:
        dbType = "datetime";
        break;

    case QVariant::Date:
        dbType = "date";
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

QT_END_NAMESPACE
