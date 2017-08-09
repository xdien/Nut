#ifndef MAINTEST_H
#define MAINTEST_H

#include <QtCore/QObject>
#include <QtCore/qglobal.h>

#include "weblogdatabase.h"
class Post;
class MainTest : public QObject
{
    Q_OBJECT
    WeblogDatabase db;
    int postId;
    Post *post;
    Query<Post> *q;
public:
    explicit MainTest(QObject *parent = 0);

signals:

private slots:
    void initTestCase();

    void cmd1();
    void cmd2();
};

#endif // MAINTEST_H
