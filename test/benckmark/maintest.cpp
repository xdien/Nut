#include <QtTest>
#include <QJsonDocument>
#include <QSqlError>

#include "consts.h"

#include "maintest.h"
#include "query.h"
#include "tableset.h"
#include "tablemodel.h"
#include "databasemodel.h"

#include "post.h"
#include "comment.h"

MainTest::MainTest(QObject *parent) : QObject(parent)
{

}

void MainTest::initTestCase()
{
    qDebug() << "User type id:" << qRegisterMetaType<Post*>();
    qDebug() << "Comment type id:" << qRegisterMetaType<Comment*>();
    qDebug() << "DB type id:" << qRegisterMetaType<WeblogDatabase*>();

    db.setDriver(DRIVER);
    db.setHostName(HOST);
    db.setDatabaseName(DATABASE);
    db.setUserName(USERNAME);
    db.setPassword(PASSWORD);

    bool ok = db.open();

    QTEST_ASSERT(ok);

    QTEST_ASSERT(ok);
}

void MainTest::insert1kPost()
{
    QTime t;
    t.start();

    for (int i = 0; i < 1000; ++i) {
        Post *newPost = new Post;
        newPost->setTitle("post title");
        newPost->setSaveDate(QDateTime::currentDateTime());

        db.posts()->append(newPost);
    }
    db.saveChanges();

    qDebug("1k post inserted in %d ms", t.elapsed());
}

QTEST_MAIN(MainTest)
