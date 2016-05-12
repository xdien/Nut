Nut
===

Advanced, Powerful and easy to use ORM for Qt5
---

Features:

 - Easy to use
 - Automatically create and update database
 - IDE auto complete support
 - Table join detect 

Sample code for reading:
---
```cpp
    auto q = FROM(db.posts())
            WHERE(Post::id() == %1)
            BIND(postId);
    auto posts = q->toList();
    // now posts is a QList<Post*> contain all posts in
    //  database that has id equal to postId variable
```
