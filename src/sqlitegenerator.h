#ifndef SQLITEGENERATOR_H
#define SQLITEGENERATOR_H

#include <QtCore/qglobal.h>
#include "sqlgeneratorbase_p.h"

class SqliteGenerator : public SqlGeneratorBase
{
public:
    SqliteGenerator();

    QString getColumnDef(Field *field);
};

#endif // SQLITEGENERATOR_H
