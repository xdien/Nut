#ifndef QUERYBASE_H
#define QUERYBASE_H

#include <QtCore/QObject>
#include <QtCore/qglobal.h>
#include "defines.h"

NUT_BEGIN_NAMESPACE

class QueryBase : public QObject
{
    Q_OBJECT
public:
    explicit QueryBase(QObject *parent = 0);

signals:

public slots:
};

NUT_END_NAMESPACE

#endif // QUERYBASE_H
