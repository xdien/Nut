#ifndef TDATABASE_H
#define TDATABASE_H

#include "database.h"

#ifdef NUT_NAMESPACE
using namespace NUT_NAMESPACE;
#endif

class Post;
class Comment;
class WeblogDatabase : public Database
{
    Q_OBJECT

    NUT_DB_VERSION(1, 1)

    NUT_DECLARE_TABLE(Post, post)

    NUT_DECLARE_TABLE(Comment, comment)

public:
    WeblogDatabase();
};


#endif // TDATABASE_H
