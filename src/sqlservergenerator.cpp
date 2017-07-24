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

#include "sqlservergenerator.h"
#include "table.h"
#include "tablemodel.h"

#include <QPoint>
#include <QRegularExpression>

NUT_BEGIN_NAMESPACE

SqlServerGenerator::SqlServerGenerator(Database *parent) : SqlGeneratorBase(parent)
{

}

QString SqlServerGenerator::masterDatabaseName(QString databaseName)
{
    return databaseName.replace(QRegularExpression("DATABASE\\=(\\w+)", QRegularExpression::CaseInsensitiveOption), "DATABASE=");
}

QString SqlServerGenerator::fieldType(FieldModel *field)
{
    QString dbType;

    switch (field->type) {
    case QVariant::Bool:
        dbType = "BIT";
        break;
    case QVariant::ByteArray:
        dbType = "VARBINARY";

        if(field->length)
            dbType.append(" (" + QString::number(field->length) + ")");
        else
            dbType.append(" (MAX)");
        break;
    case QVariant::Date:
        dbType = "DATE";
        break;
    case QVariant::DateTime:
        dbType = "DATETIME";
        break;
    case QVariant::Time:
        dbType = "TIME";
        break;
    case QVariant::Double:
        dbType = "REAL";
        break;
    case QVariant::Int:
        dbType = "INT";
        if(field->isAutoIncrement)
            dbType += " IDENTITY(1,1)";
        break;

    case QVariant::Point:
    case QVariant::PointF:
        dbType = "GEOMETRY";
        break;

    case QVariant::String:
        if(field->length)
            dbType = QString("NVARCHAR(%1)").arg(field->length);
        else
            dbType = "NVARCHAR(MAX)";
        break;

    case QVariant::Uuid:
        dbType = "UNIQUEIDENTIFIER";
        break;

    default:
        dbType = "";
    }

    return dbType;
}

QString SqlServerGenerator::diff(FieldModel *oldField, FieldModel *newField)
{
    QString sql = "";
    if(oldField && newField)
        if(*oldField == *newField)
            return QString::null;

    if(!newField){
        sql = "DROP COLUMN " + oldField->name;
    }else{
        if(oldField)
            sql = "MODIFY COLUMN ";
        else
            sql = "ADD ";

        sql.append(fieldDeclare(newField));
    }
    return sql;
}

QString SqlServerGenerator::escapeValue(const QVariant &v) const
{
    if(v.type() == QVariant::String || v.type() == QVariant::Char)
        return "N'" + v.toString() + "'";
    else if (v.type() == QVariant::Point) {
        QPoint pt = v.toPoint();
        return QString("geography::POINT(%1, %2, 4326)").arg(pt.x()).arg(pt.y());
    } else if (v.type() == QVariant::Point) {
        QPointF pt = v.toPointF();
        return QString("geography::POINT(%1, %2, 4326)").arg(pt.x()).arg(pt.y());
    }
        return SqlGeneratorBase::escapeValue(v);
}

NUT_END_NAMESPACE
