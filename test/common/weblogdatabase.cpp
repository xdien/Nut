#include <QDebug>

#include "post.h"
#include "comment.h"
#include "weblogdatabase.h"

WeblogDatabase::WeblogDatabase() : Database(), m_posts(new TableSet<Post>(this)), m_comments(new TableSet<Comment>(this))
{
}
