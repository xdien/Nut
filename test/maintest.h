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

    void dataScheema();
    void createPost();
    void selectPosts();
    void testDate();
    void selectWithInvalidRelation();
    void modifyPost();
    void deletePost();
};

#endif // MAINTEST_H
