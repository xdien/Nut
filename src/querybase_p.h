#ifndef QUERYBASE_H
#define QUERYBASE_H

#include <QtCore/QObject>
#include <QtCore/qglobal.h>

class QueryBase : public QObject
{
    Q_OBJECT
public:
    explicit QueryBase(QObject *parent = 0);

signals:

public slots:
};

#endif // QUERYBASE_H
