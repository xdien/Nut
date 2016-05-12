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
class QJsonObject;
class TableScheema;

struct Field{
    Field() : name(QString::null), length(0), defaultValue(QString::null),
        notNull(false), isPrimaryKey(false), isAutoIncrement(false)
    {

    }

    QString name;
    QVariant::Type type;
    int length;
    QString defaultValue;
    bool notNull;
    bool isPrimaryKey;
    bool isAutoIncrement;

    bool operator ==(const Field &f) const{

        bool b = name == f.name
                && type == f.type
                && length == f.length
                && defaultValue == f.defaultValue
                && notNull == f.notNull;

        return b;
    }

    bool operator !=(const Field &f) const{
        return !(*this == f);
    }
};

struct Relation{
    QString className;
    QString localColumn;
    TableScheema *table;
    QString foregionColumn;
};
class TableScheema
{
public:

    TableScheema(int typeId, QString tableName);
    TableScheema(QJsonObject json, QString tableName);

    QJsonObject toJson() const;

//    static TableScheema *registerTable(int typeId, QString tableName);
//    static void createForegionKeys();
//    static TableScheema* scheema(QString className);

    Field *field(QString name) const;
    Relation *foregionKey(QString otherTable) const;

    QString toString() const;

    QString primaryKey() const;

    QString name() const;
    void setName(const QString &name);

    QString className() const;
    void setClassName(const QString &className);

    int typeId() const;
    void setTypeId(const int &typeId);
    QList<Field *> fields() const;
    QList<Relation *> foregionKeys() const;
    QStringList fieldsNames() const;

    static TableScheema *findByTypeId(int typeId);
    static TableScheema *findByName(QString name);
    static TableScheema *findByClassName(QString className);

    bool operator ==(const TableScheema &t) const;
    bool operator !=(const TableScheema &t) const;

private:
    QString _name;
    QString _className;
    int _typeId;
    QList<Field*> _fields;
    QList<Relation*> _foregionKeys;
    static QSet<TableScheema*>_allModels;
};

#endif // TABLESCHEEMA_H
