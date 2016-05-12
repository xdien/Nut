#ifndef MYSQLGENERATOR_H
#define MYSQLGENERATOR_H

#include <QtCore/qglobal.h>
#include "sqlgeneratorbase_p.h"

QT_BEGIN_NAMESPACE

class MySqlGenerator : public SqlGeneratorBase
{
public:
    MySqlGenerator();

    QString getColumnDef(Field *field);

};

QT_END_NAMESPACE

#endif // MYSQLGENERATOR_H
