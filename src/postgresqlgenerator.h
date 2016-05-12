#ifndef POSTGRESQLGENERATOR_H
#define POSTGRESQLGENERATOR_H

#include <QtCore/qglobal.h>
#include "sqlgeneratorbase_p.h"

QT_BEGIN_NAMESPACE

class Field;
class PostgreSqlGenerator : public SqlGeneratorBase
{
public:
    PostgreSqlGenerator();

    QString getColumnDef(Field *field);
//    QString saveSql(Table *t, QString tableName);

    QString deleteTableRows(QString tableName, QString where);
};

QT_END_NAMESPACE

#endif // POSTGRESQLGENERATOR_H
