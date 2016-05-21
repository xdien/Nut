#ifndef TDATABASE_H
#define TDATABASE_H

#include "database.h"

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
