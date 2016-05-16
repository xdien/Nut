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

#include <QtCore/QMetaObject>
#include <QtCore/QMetaProperty>
#include <QtCore/QDebug>

#include <QJsonArray>
#include <QJsonObject>

#include "tablescheema.h"
#include "defines_p.h"


QSet<TableScheema*> TableScheema::_allModels;
//QMap<int, TableScheema*> TableScheema::scheemas;

QString TableScheema::name() const
{
    return _name;
}

void TableScheema::setName(const QString &name)
{
    _name = name;
}

QString TableScheema::className() const
{
    return _className;
}

void TableScheema::setClassName(const QString &className)
{
    _className = className;
}

int TableScheema::typeId() const
{
    return _typeId;
}

void TableScheema::setTypeId(const int &typeId)
{
    _typeId = typeId;
}

Field *TableScheema::field(QString name) const
{
    foreach (Field *f, _fields)
        if(f->name == name)
            return f;
    
    return 0;
}

QList<Field *> TableScheema::fields() const
{
    return _fields;
}

QList<Relation *> TableScheema::foregionKeys() const
{
    return _foregionKeys;
}

QStringList TableScheema::fieldsNames() const
{
    QStringList ret;
    foreach (Field *f, _fields)
        ret.append(f->name);
    return ret;
}

TableScheema *TableScheema::findByTypeId(int typeId)
{
    foreach (TableScheema *model, _allModels)
        if(model->typeId() == typeId)
            return model;
    return 0;
}

TableScheema *TableScheema::findByName(QString name)
{
    foreach (TableScheema *model, _allModels)
        if(model->name() == name)
            return model;
    return 0;
}

TableScheema *TableScheema::findByClassName(QString className)
{
    foreach (TableScheema *model, _allModels)
        if(model->className() == className)
            return model;
    return 0;
}

bool TableScheema::operator ==(const TableScheema &t) const{
    if(_name != t.name())
        return false;

    foreach (Field *f, _fields) {
        Field *tf = t.field(f->name);
        if(!tf)
            return false;

        if(*f != *tf)
            return false;
    }

    return true;
}

bool TableScheema::operator !=(const TableScheema &t) const
{
    return !(*this == t);
}

TableScheema::TableScheema(int typeId, QString tableName)
{
    const QMetaObject *tableMetaObject = QMetaType::metaObjectForType(typeId);

    _typeId = typeId;
    _name = tableName;
    _className = tableMetaObject->className();

    // get fields names
    for(int j = 0; j < tableMetaObject->classInfoCount(); j++){
        QString name = tableMetaObject->classInfo(j).name();

        name = name.remove(__nut_NAME_PERFIX);

        if(name.contains(" ")){
            QStringList parts = name.split(" ");
            QString propName = parts.at(1);

            if(propName == __nut_FIELD){
                Field *f = new Field;
                f->name = parts.at(0);
                _fields.append(f);
            }
        }
    }
    // Browse all fields
    for(int j = 1; j < tableMetaObject->propertyCount(); j++){
        QMetaProperty fieldProperty = tableMetaObject->property(j);

        Field *f = field(fieldProperty.name());
        if(!f)
            continue;

        f->type = fieldProperty.type();

        _fields.append(f);
    }

    // Browse class infos
    for(int j = 0; j < tableMetaObject->classInfoCount(); j++){
        QString name = tableMetaObject->classInfo(j).name();
        QString value = tableMetaObject->classInfo(j).value();

        name = name.remove(__nut_NAME_PERFIX);


        if(name.contains(" ")){
            QStringList parts = name.split(" ");
            QString propName = parts.at(1);

            if(propName == __nut_FOREGION_KEY){
                Relation *fk = new Relation;
                fk->localColumn = parts.at(0);
                fk->foregionColumn = value;
                fk->className = value;
                _foregionKeys.append(fk);
            }

            if(propName == __nut_FIELD){

            }


            Field *f = field(parts.at(0));
            if(!f)
                continue;

            if(propName == __nut_LEN)
                f->length = value.toInt();
            else if(propName == __nut_NOT_NULL)
                f->notNull = false;
            else if(propName == __nut_DEFAULT_VALUE)
                f->defaultValue = value;
            else if(propName == __nut_PRIMARY_KEY)
                f->isPrimaryKey = true;
            else if(propName == __nut_AUTO_INCREMENT)
                f->isAutoIncrement = true;

        }
    }

    _allModels.insert(this);
}

/*
 "comment": {
        "auto_increment": "id",
        "fields": {
            "id": {
                "name": "id",
                "type": "int"
            },
            "userId": {
                "name": "userId",
                "type": "int"
            }
        },
        "primary_key": "id"
    },
*/
TableScheema::TableScheema(QJsonObject json, QString tableName)
{
    _name = tableName;

    QJsonObject fields = json.value(__FIELDS).toObject();
    foreach (QString key, fields.keys()) {
        QJsonObject fieldObject = fields.value(key).toObject();
        Field *f = new Field;
        f->name = fieldObject.value(__NAME).toString();
        f->type = QVariant::nameToType(fieldObject.value(__TYPE).toString().toLatin1().data());

        if(fieldObject.contains(__nut_NOT_NULL))
            f->notNull = fieldObject.value(__nut_NOT_NULL).toBool();

        if(fieldObject.contains(__nut_LEN))
            f->length = fieldObject.value(__nut_LEN).toInt();

        if(fieldObject.contains(__nut_DEFAULT_VALUE))
            f->defaultValue = fieldObject.value(__nut_DEFAULT_VALUE).toString();
        _fields.append(f);
    }

    if(json.keys().contains(__nut_AUTO_INCREMENT))
        field(json.value(__nut_AUTO_INCREMENT).toString())->isAutoIncrement = true;

    if(json.keys().contains(__nut_PRIMARY_KEY))
        field(json.value(__nut_PRIMARY_KEY).toString())->isAutoIncrement = true;

}

QJsonObject TableScheema::toJson() const
{
    QJsonObject obj;
    QJsonObject fieldsObj;

    foreach (Field *f, _fields) {
        QJsonObject fieldObj;
        fieldObj.insert(__NAME, f->name);
        fieldObj.insert(__TYPE, QVariant::typeToName(f->type));

        if(f->length)
            fieldObj.insert(__nut_LEN, f->length);

        if(f->notNull)
            fieldObj.insert(__nut_NOT_NULL, f->notNull);

        if(!f->defaultValue.isNull())
            fieldObj.insert(__nut_DEFAULT_VALUE, f->defaultValue);

        fieldsObj.insert(f->name, fieldObj);

        if(f->isAutoIncrement)
            obj.insert(__nut_PRIMARY_KEY, f->name);

        if(f->isPrimaryKey)
            obj.insert(__nut_AUTO_INCREMENT, f->name);
    }
    obj.insert(__FIELDS, fieldsObj);

    return obj;
}

//TableScheema *TableScheema::registerTable(int typeId, QString tableName)
//{
//    TableScheema *scheema = new TableScheema(typeId, tableName);
//    scheemas.insert(typeId, scheema);
//    return scheema;
//}

//void TableScheema::createForegionKeys()
//{
//    foreach (TableScheema *sch, scheemas) {
//        foreach (ForegionKey *fk, sch->_foregionKeys) {
//            fk->table = scheema(fk->tableName);
//        }
//    }
//}

//TableScheema *TableScheema::scheema(QString className)
//{
//    foreach (TableScheema *s, scheemas)
//        if(s->_className == className)
//            return s;
//    return 0;
//}

Relation *TableScheema::foregionKey(QString otherTable) const
{
    foreach (Relation *fk, _foregionKeys)
        if(fk->className == otherTable)
            return fk;

    return 0;
}

QString TableScheema::toString() const
{
    QStringList sl;
    foreach (Field *f, _fields)
        sl.append(f->name + " " + QVariant::typeToName(f->type));

    QString ret = QString("%1 (%2)")
            .arg(_name)
            .arg(sl.join(", "));
    return ret;
}

QString TableScheema::primaryKey() const
{
    foreach (Field *f, _fields)
        if(f->isPrimaryKey)
            return f->name;
    return QString::null;
}
