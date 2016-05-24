#include <QtTest>
#include <QJsonDocument>
#include <QSqlError>

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

    //sql server
//    db.setDriver("QODBC");
//    db.setHostName("127.0.0.1");
//    db.setDatabaseName("DRIVER={SQL Server};Server=.;Database=Nut;Uid=sa;Port=1433;Pwd=qwe123!@#;WSID=.");
//    db.setUserName("sa");
//    db.setPassword("qwe123!@#");

    // postgres
    db.setDriver("QPSQL");
    db.setHostName("127.0.0.1");
    db.setDatabaseName("nutdb3");
    db.setUserName("postgres");
    db.setPassword("856856");

// mysql
//    db.setDriver("QMYSQL");
//    db.setHostName("127.0.0.1");
//    db.setDatabaseName("nutdb");
//    db.setUserName("root");
//    db.setPassword("onlyonlyi");

    bool ok = db.open();

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
