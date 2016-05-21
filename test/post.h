#ifndef USER_H
#define USER_H

#include <QtCore/qglobal.h>
#include "table.h"
#include "database.h"
#include "comment.h"
#include "databasemodel.h"

class Post : public Table
{
    Q_OBJECT

    NUT_PRIMARY_AUTO_INCREMENT(id)
    NUT_DECLARE_FIELD(int, id, id, setId)

    NUT_NOT_NULL(title)
    NUT_LEN(title, 50)
    NUT_DECLARE_FIELD(QString, title, title, setTitle)

    NUT_DECLARE_FIELD(QDateTime, saveDate, saveDate, setSaveDate)

    NUT_DECLARE_FIELD(QString, body, body, setBody)

    NUT_DECLARE_CHILD_TABLE(Comment, comments)

public:
    explicit Post(QObject *tableSet = 0);

signals:

public slots:
};

//Q_DECLARE_METATYPE(User*)
#endif // USER_H
