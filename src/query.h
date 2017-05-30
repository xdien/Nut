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

#ifndef QUERY_H
#define QUERY_H

#include <QtCore/QVariant>
#include <QtCore/QDebug>
#include <QtCore/QScopedPointer>
#include <QtCore/QRegularExpression>

#include "query_p.h"
#include "database.h"
#include "databasemodel.h"
#include "tablesetbase_p.h"
#include "sqlgeneratorbase_p.h"
#include "querybase_p.h"
#include "wherephrase.h"
#include "tablemodel.h"

NUT_BEGIN_NAMESPACE

template<class T>
class NUT_EXPORT Query : public QueryBase
{
    QueryPrivate *d_ptr;
    Q_DECLARE_PRIVATE(Query)

public:
    Query(Database *database, TableSetBase *tableSet);

    ~Query();

    int remove();
    T *first();

    int count();

    QList<T *> toList(int count = -1);

    template<typename F>
    QList<F> select(const FieldPhrase<F> f);
    QVariant max(FieldPhrase<int> &f);
    QVariant min(FieldPhrase<int> &f);
    QVariant average(FieldPhrase<int> &f);

    Query<T> *join(const QString &tableName);
    Query<T> *setWhere(WherePhrase where);

    Query<T> *join(Table *c){
        join(c->metaObject()->className());
        return this;
    }

//    Query<T> *setWhere(const QString &where);
    Query<T> *orderBy(QString fieldName, QString type);
    Query<T> *orderBy(WherePhrase phrase);

    int update(WherePhrase phrase);
};

template <typename T>
inline Query<T> *createQuery(TableSet<T> *tableSet){

}

template<class T>
Q_OUTOFLINE_TEMPLATE Query<T>::Query(Database *database, TableSetBase *tableSet) : QueryBase(database),
    d_ptr(new QueryPrivate(this))
{
    Q_D(Query);

    d->database = database;
    d->tableSet = tableSet;
    d->tableName = //TableModel::findByClassName(T::staticMetaObject.className())->name();
                d->database->model().modelByClass(T::staticMetaObject.className())->name();
}

template<class T>
Q_OUTOFLINE_TEMPLATE Query<T>::~Query()
{
    Q_D(Query);
    delete d;
}

template<class T>
Q_OUTOFLINE_TEMPLATE QList<T *> Query<T>::toList(int count)
{
    Q_D(Query);
    QList<T*> result;
    d->select = "*";

//    QSqlQuery q = d->database->exec(d->database->sqlGenertor()->selectCommand(d->wheres, d->orders, d->tableName, d->joinClassName));
    QString sql = d->database->sqlGenertor()->selectCommand(
                    SqlGeneratorBase::SelectAll,
                    "",
                    d->wheres,
                    d->orderPhrases,
                    d->tableName,
                    d->joinClassName);
    QSqlQuery q = d->database->exec(sql);

//    QString pk = TableModel::findByName(d->tableName)->primaryKey();
    QString pk = d->database->model().model(d->tableName)->primaryKey();
    QVariant lastPkValue = QVariant();
    int childTypeId = 0;
    T *lastRow = 0;
    TableSetBase *childTableSet;

    //FIXME: getting table error
//    QStringList masterFields = TableModel::findByName(d->tableName)->fieldsNames();
    QStringList masterFields = d->database->model().model(d->tableName)->fieldsNames();
    QStringList childFields;
    if(!d->joinClassName.isNull()) {
        TableModel *joinTableModel = TableModel::findByClassName(d->joinClassName);
        if(joinTableModel){
//            childFields = d->database->model().modelByClass(d->joinClassName)->fieldsNames();
            childFields = TableModel::findByClassName(d->joinClassName)->fieldsNames();
            QString joinTableName = d->database->tableName(d->joinClassName);
            childTypeId = d->database->model().model(joinTableName)->typeId();
//            childTypeId = TableModel::findByName(joinTableName)->typeId();
        }
    }

    while (q.next()) {
        if(lastPkValue != q.value(pk)){
            T *t = new T();

            foreach (QString field, masterFields)
                t->setProperty(field.toLatin1().data(), q.value(field));

            t->setTableSet(d->tableSet);
            t->setStatus(Table::FeatchedFromDB);
            t->setParent(this);

            result.append(t);
            lastRow = t;

            if(childTypeId){
                QSet<TableSetBase*> tableSets = t->tableSets;
                foreach (TableSetBase *ts, tableSets)
                    if(ts->childClassName() == d->joinClassName)
                        childTableSet = ts;
            }
        }

        if(childTypeId){
            const QMetaObject *childMetaObject = QMetaType::metaObjectForType(childTypeId);
            Table *childTable = qobject_cast<Table*>(childMetaObject->newInstance());

            foreach (QString field, childFields)
                childTable->setProperty(field.toLatin1().data(), q.value(field));
//TODO: set database for table
            childTable->setParent(this);
            childTable->setParentTable(lastRow);
            childTable->setStatus(Table::FeatchedFromDB);
            childTable->setTableSet(childTableSet);
            childTableSet->add(childTable);
        }
        lastPkValue = q.value(pk);

        if(!--count)
            break;
    }

    deleteLater();
    return result;
}

template <typename T>
template <typename F>
Q_OUTOFLINE_TEMPLATE QList<F> Query<T>::select(const FieldPhrase<F> f)
{
    Q_D(Query);
    QList<F> ret;
    QString sql = d->database->sqlGenertor()->selectCommand(
                    SqlGeneratorBase::SignleField,
                    f.data()->text,
                    d->wheres,
                    d->orderPhrases,
                    d->tableName,
                    d->joinClassName);
    QSqlQuery q = d->database->exec(sql);

    while (q.next()) {
        QVariant v = q.value(f.data()->text);
        ret.append(v.value<F>());
    }

    deleteLater();
    return ret;
}

template<class T>
Q_OUTOFLINE_TEMPLATE T *Query<T>::first()
{
    QList<T *> list = toList(1);

    if(list.count())
        return list.first();
    else
        return 0;
}

template<class T>
Q_OUTOFLINE_TEMPLATE int Query<T>::count()
{
    Q_D(Query);

    d->select = "COUNT(*)";
    QSqlQuery q = d->database->exec(d->database->sqlGenertor()->selectCommand("COUNT(*)", d->wheres, d->orders, d->tableName, d->joinClassName));

    qDebug() << "sql="<<d->database->sqlGenertor()->selectCommand("COUNT(*)", d->wheres, d->orders, d->tableName, d->joinClassName);
    if(q.next())
        return q.value(0).toInt();
    return 0;
}

template<class T>
Q_OUTOFLINE_TEMPLATE QVariant Query<T>::max(FieldPhrase<int> &f){
    Q_D(Query);

    QSqlQuery q = d->database->exec(d->database->sqlGenertor()->selectCommand("MAX(" + f.data()->text + ")", d->wheres, d->orders, d->tableName, d->joinClassName));

    if(q.next())
        return q.value(0).toInt();
    return 0;
}

template<class T>
Q_OUTOFLINE_TEMPLATE QVariant Query<T>::min(FieldPhrase<int> &f){
    Q_D(Query);

    QSqlQuery q = d->database->exec(d->database->sqlGenertor()->selectCommand("MIN(" + f.data()->text + ")", d->wheres, d->orders, d->tableName, d->joinClassName));

    if(q.next())
        return q.value(0).toInt();
    return 0;
}

template<class T>
Q_OUTOFLINE_TEMPLATE QVariant Query<T>::average(FieldPhrase<int> &f)
{
    Q_D(Query);

    QSqlQuery q = d->database->exec(d->database->sqlGenertor()->selectCommand("AVG(" + f.data()->text + ")", d->wheres, d->orders, d->tableName, d->joinClassName));

    if(q.next())
        return q.value(0).toInt();
    return 0;
}

template<class T>
Q_OUTOFLINE_TEMPLATE int Query<T>::remove()
{
    Q_D(Query);

    QString sql = d->database->sqlGenertor()->deleteCommand(d->wheres, d->tableName);
//            d->_database->sqlGenertor()->deleteRecords(_tableName, queryText());
//    sql = compileCommand(sql);
    QSqlQuery q = d->database->exec(sql);
    return q.numRowsAffected();
}

template<class T>
Q_OUTOFLINE_TEMPLATE Query<T> *Query<T>::join(const QString &tableName)
{
    Q_D(Query);
    d->joinClassName = tableName;
    return this;
}

template<class T>
Q_OUTOFLINE_TEMPLATE Query<T> *Query<T>::setWhere(WherePhrase where)
{
    Q_D(Query);
    d->wheres.append(where);
    return this;
}

template<class T>
Q_OUTOFLINE_TEMPLATE Query<T> *Query<T>::orderBy(QString fieldName, QString type)
{
    Q_D(Query);
    d->orders.insert(fieldName, type);
    return this;
}

template<class T>
Q_OUTOFLINE_TEMPLATE Query<T> *Query<T>::orderBy(WherePhrase phrase)
{
    Q_D(Query);
    d->orderPhrases.append(phrase);
    return this;
}

template<class T>
Q_OUTOFLINE_TEMPLATE int Query<T>::update(WherePhrase phrase)
{
    Q_D(Query);

    QString sql = d->database->sqlGenertor()->updateCommand(
                phrase,
                d->wheres,
                d->tableName);
    qDebug() << sql;
    QSqlQuery q = d->database->exec(sql);
    return q.numRowsAffected();
}

NUT_END_NAMESPACE

#endif // QUERY_H
