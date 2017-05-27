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

#ifndef TABLESCHEEMA_H
#define TABLESCHEEMA_H

#include <QtCore/QVariant>
#include <QDebug>
#include "defines.h"

class QJsonObject;

NUT_BEGIN_NAMESPACE

class TableModel;
struct FieldModel{
    FieldModel() : name(QString::null), length(0), defaultValue(QString::null),
        notNull(false), isPrimaryKey(false), isAutoIncrement(false), isUnique(false)
    {

    }

    QString name;
    QVariant::Type type;
    QString typeName;
    int length;
    QString defaultValue;
    bool notNull;
    bool isPrimaryKey;
    bool isAutoIncrement;
    bool isUnique;

    bool operator ==(const FieldModel &f) const{

        bool b = name.toLower() == f.name.toLower()
                && type == f.type
                && length == f.length
                && defaultValue == f.defaultValue
                && notNull == f.notNull;

        return b;
    }

    bool operator !=(const FieldModel &f) const{
        return !(*this == f);
    }
};

struct RelationModel{
    QString className;
    QString localColumn;
    TableModel *table;
    QString foregionColumn;
};
class TableModel
{
public:

    TableModel(int typeId, QString tableName);
    TableModel(QJsonObject json, QString tableName);

    QJsonObject toJson() const;

//    static TableScheema *registerTable(int typeId, QString tableName);
//    static void createForegionKeys();
//    static TableModel* model(QString className);

    FieldModel *field(QString name) const;
    RelationModel *foregionKey(QString otherTable) const;

    QString toString() const;

    QString primaryKey() const;

    QString name() const;
    void setName(const QString &name);

    QString className() const;
    void setClassName(const QString &className);

    int typeId() const;
    void setTypeId(const int &typeId);
    QList<FieldModel *> fields() const;
    QList<RelationModel *> foregionKeys() const;
    QStringList fieldsNames() const;

    static QSet<TableModel *> allModels();
    static TableModel *findByTypeId(int typeId);
    static TableModel *findByName(QString name);
    static TableModel *findByClassName(QString className);

    bool operator ==(const TableModel &t) const;
    bool operator !=(const TableModel &t) const;

private:
    QString _name;
    QString _className;
    int _typeId;
    QList<FieldModel*> _fields;
    QList<RelationModel*> _foregionKeys;
    static QSet<TableModel*>_allModels;
};

NUT_END_NAMESPACE

#endif // TABLESCHEEMA_H
