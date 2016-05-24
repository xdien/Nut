QT       += qml quick testlib sql
QT       -= gui

TARGET = tst_nut
TEMPLATE = app

CONFIG   += warn_on qmltestcase c++11
INCLUDEPATH += $$PWD/../../src $$PWD/../common
include(../../nut.pri)
IMPORTPATH += $$OUT_PWD/../src/imports
SOURCES += \
    maintest.cpp \
    ../common/comment.cpp \
    ../common/post.cpp \
    ../common/weblogdatabase.cpp

HEADERS += \
    maintest.h \
    ../common/comment.h \
    ../common/post.h \
    ../common/weblogdatabase.h
