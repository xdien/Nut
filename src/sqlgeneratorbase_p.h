#ifndef SQLGENERATORBASE_H
#define SQLGENERATORBASE_H

#include <QtCore/qglobal.h>

#include <QStringList>

QT_BEGIN_NAMESPACE

class Table;
class Field;
class DatabaseModel;
class TableScheema;
class SqlGeneratorBase
{
public:
    SqlGeneratorBase();
    virtual ~SqlGeneratorBase();

    virtual QString saveSql(Table *t, QString tableName);
    virtual QString getColumnDef(Field *field) = 0;
    virtual QStringList getDiff(DatabaseModel lastModel, DatabaseModel newModel);
    virtual QString getDiff(Field *oldField, Field *newField);
    virtual QString getDiff(TableScheema *oldTable, TableScheema *newTable);


    virtual QString insertCommand(Table *t, QString tableName);
    virtual QString updateCommand(Table *t, QString tableName);
    virtual QString deleteCommand(Table *t, QString tableName);

    virtual QString deleteTableRows(QString tableName, QString where);

};

QT_END_NAMESPACE

#endif // SQLGENERATORBASE_H
