#include <QtTest>
#include <QJsonDocument>
#include <QSqlError>

#include "maintest.h"
#include "query.h"
#include "tableset.h"
#include "tablescheema.h"
#include "databasemodel.h"

#include "post.h"
#include "comment.h"

MainTest::MainTest(QObject *parent) : QObject(parent)
{

}

void MainTest::initTestCase()
{
    qDebug() << "Table type id:" << qRegisterMetaType<Table*>();
    qDebug() << "User type id:" << qRegisterMetaType<Post*>();
    qDebug() << "Comment type id:" << qRegisterMetaType<Comment*>();
    qDebug() << "DB type id:" << qRegisterMetaType<WeblogDatabase*>();

    db.setDriver("QSQLITE");
    db.setHostName("127.0.0.1");
    db.setDatabaseName("nutdb");
//    db.setUserName("postgres");
//    db.setPassword("856856");
//    db.setUserName("root");
//    db.setPassword("onlyonlyi");
    bool ok = db.open();

    QTEST_ASSERT(ok);
}

void MainTest::dataScheema()
{
    auto json = db.model().toJson();
    auto model = DatabaseModel::fromJson(json);

    //    qDebug() << model.toJson();
    //    qDebug() << db.model().toJson();
    QTEST_ASSERT(model == db.model());
}

void MainTest::createPost()
{
    Post *newPost = new Post;
    newPost->setTitle("post title");

    db.posts()->append(newPost);

    for(int i = 0 ; i < 3; i++){
        Comment *comment = new Comment;
        comment->setMessage("comment #" + QString::number(i));

        newPost->comments()->append(comment);
    }
    db.saveChanges();

    postId = newPost->id();

    QTEST_ASSERT(newPost->id() != 0);
    qDebug() << "New post inserted with id:" << newPost->id();
}

void MainTest::selectPosts()
{
    auto q = FROM(db.posts())
            JOIN(Comment)
            WHERE(Post::id() == %1)
            BIND(postId);
    auto posts = q->toList();

    QTEST_ASSERT(posts.length() == 1);
    QTEST_ASSERT(posts.at(0)->comments()->length() == 3);
    QTEST_ASSERT(posts.at(0)->title() == "post title");

    qDebug()<<posts.at(0)->comments()->at(0)->message();
    QTEST_ASSERT(posts.at(0)->comments()->at(0)->message() == "comment #0");
    QTEST_ASSERT(posts.at(0)->comments()->at(1)->message() == "comment #1");
    QTEST_ASSERT(posts.at(0)->comments()->at(2)->message() == "comment #2");
}

void MainTest::selectWithInvalidRelation()
{
    auto q = FROM(db.posts())
            JOIN(Invalid_Class_Name)
            SELECT();
}

void MainTest::modifyPost()
{
    auto q = FROM(db.posts())
            WHERE(Post::id() == %1)
            BIND(postId);

    Post *post = q->first();

    QTEST_ASSERT(post != 0);

    post->setTitle("new name");
    db.saveChanges();

    q = FROM(db.posts())
            WHERE(Post::id() == %1)
            BIND(postId);
    post = q->first();
    QTEST_ASSERT(post->title() == "new name");
}

void MainTest::deletePost()
{
    auto count = FROM(db.posts())
            WHERE(Post::id() == %1)
            BIND(postId)
            DELETE();

    QTEST_ASSERT(count == 1);

    count = FROM(db.posts())
            WHERE(Post::id() == %1)
            BIND(postId)
            COUNT();

    QTEST_ASSERT(count == 0);
}

QTEST_MAIN(MainTest)
