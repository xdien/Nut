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

#ifndef SYNTAX_DEFINES_H
#define SYNTAX_DEFINES_H

#define NUT_NAMESPACE Nut

#include "defines_p.h"
#include "qglobal.h"


#ifdef NUT_COMPILE_STATIC
#   define NUT_EXPORT
#else
#   define NUT_EXPORT Q_DECL_EXPORT
#endif


#ifdef NUT_NAMESPACE
#   define __NUT_NAMESPACE_PERFIX NUT_NAMESPACE::
#   define NUT_WRAP_NAMESPACE(x) NUT_NAMESPACE::x
#else
#   define __NUT_NAMESPACE_PERFIX
#   define NUT_WRAP_NAMESPACE(x) x
#endif

// Database
//TODO: remove minor version
#define NUT_DB_VERSION(major, minor) Q_CLASSINFO(QT_STRINGIFY(__nut_NAME_PERFIX __nut_DB_VERSION), QT_STRINGIFY(#major "." #minor))

#define NUT_DECLARE_TABLE(type, name)                                       \
    Q_CLASSINFO(QT_STRINGIFY(__nut_NAME_PERFIX __nut_TABLE " "  #type), #name)                              \
    Q_PROPERTY(type* name READ name)                                        \
    Q_PROPERTY(NUT_WRAP_NAMESPACE(TableSet<type>) name##s READ name##s)                         \
    type* m_##name;                                                         \
    NUT_WRAP_NAMESPACE(TableSet<type>) *m_##name##s;                                            \
public:                                                                     \
    static const type _##name;                                              \
    type* name() const{ return m_##name; }                                  \
    NUT_WRAP_NAMESPACE(TableSet<type>) *name##s() const { return m_##name##s; }

//Table
#define NUT_DECLARE_FIELD(type, name, read, write)                          \
    Q_PROPERTY(type name READ read WRITE write)                             \
    Q_CLASSINFO(QT_STRINGIFY(__nut_NAME_PERFIX #name " " __nut_FIELD), #name)             \
    type m_##name;                                                          \
public:                                                                     \
    static NUT_WRAP_NAMESPACE(FieldPhrase<type>) name ## Field(){                                       \
        static NUT_WRAP_NAMESPACE(FieldPhrase<type>) f = NUT_WRAP_NAMESPACE(FieldPhrase<type>)(staticMetaObject.className(), #name);                          \
        return f;                                                           \
    }                                                                       \
    type read() const{                                                      \
        return m_##name;                                                    \
    }                                                                       \
    void write(type name){                                                  \
        m_##name = name;                                                    \
        propertyChanged(#name);                                             \
    }

#define NUT_FOREGION_KEY(type, keytype, name, read, write)                  \
    Q_PROPERTY(type* name READ read WRITE write)                            \
    NUT_DECLARE_FIELD(keytype, name##Id, read##Id, write##Id)               \
    Q_CLASSINFO(QT_STRINGIFY(__nut_NAME_PERFIX #name "Id " __nut_FOREGION_KEY), #type)    \
    type *m_##name;                                                         \
public:                                                                     \
    type *read() const { return m_##name ; }                                \
    void write(type *name){                                                 \
        m_##name = name;                                                    \
    }

#define NUT_DECLARE_CHILD_TABLE(type, n)                                    \
    private:                                                                \
        NUT_WRAP_NAMESPACE(TableSet)<type> *m_##n;                                              \
    public:                                                                 \
        static type *n##Table(){                                            \
            static type *f = new type();                                    \
            return f;                                                       \
        }                                                                   \
        NUT_WRAP_NAMESPACE(TableSet)<type> *n(){                                                \
            return m_##n;                                                   \
        }


#define NUT_INDEX(name, field, order)
#define NUT_PRIMARY_KEY(x)                  Q_CLASSINFO(QT_STRINGIFY(__nut_NAME_PERFIX #x " " __nut_PRIMARY_KEY),  #x)
#define NUT_AUTO_INCREMENT(x)               Q_CLASSINFO(QT_STRINGIFY(__nut_NAME_PERFIX #x " " __nut_AUTO_INCREMENT),  #x)
#define NUT_PRIMARY_AUTO_INCREMENT(x)       NUT_PRIMARY_KEY(x)          \
                                            NUT_AUTO_INCREMENT(x)
#define NUT_UNIQUE(x)                       Q_CLASSINFO(QT_STRINGIFY(__nut_NAME_PERFIX #x " " __nut_UNIQUE),  #x)
#define NUT_LEN(field, len)                 Q_CLASSINFO(QT_STRINGIFY(__nut_NAME_PERFIX #field " " __nut_LEN),    #len)
#define NUT_DEFAULT_VALUE(x, n)             Q_CLASSINFO(QT_STRINGIFY(__nut_NAME_PERFIX #x " " __nut_DEFAULT_VALUE),    #n)
#define NUT_NOT_NULL(x)                     Q_CLASSINFO(QT_STRINGIFY(__nut_NAME_PERFIX #x " " __nut_NOT_NULL), "1")

#ifndef NUT_NO_KEYWORDS
#   define FROM(x)          (x->query())
#   define WHERE(x)         ->setWhere(x)
#   define JOIN(x)          ->join(#x)
#   define ORDERBY(x)       ->orderBy(#x, "ASC");
#   define ORDERBY_DESC(x)  ->orderBy(#x, "DESC");

#   define SELECT()         ->toList()
#   define COUNT()          ->count()
#   define DELETE()         ->remove()
#   define FIRST()          ->first()
#endif // NUT_NO_KEYWORDS



#endif // SYNTAX_DEFINES_H
