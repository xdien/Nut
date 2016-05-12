#include "comment.h"
#include "tableset.h"
#include "post.h"

Post::Post(QObject *parent) : Table(parent),
    m_comments(new TableSet<Comment>(this)), m_id(0), m_title("")
{

}
