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
public:
    explicit MainTest(QObject *parent = 0);

signals:

private slots:
    void initTestCase();

    void dataScheema();
    void createPost();
    void createPost2();
    void selectPosts();
    void selectPostsWithoutTitle();
    void selectComments();
    void testDate();
    void selectWithInvalidRelation();
    void select10NewstPosts();
    void modifyPost();
    void deletePost();
};

#endif // MAINTEST_H
