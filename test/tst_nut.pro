QT       += qml quick testlib sql

QT       -= gui

TARGET = tst_nut
CONFIG   += warn_on qmltestcase c++11
INCLUDEPATH += $$PWD/../src
include(../nut.pri)
TEMPLATE = app
IMPORTPATH += $$OUT_PWD/../src/imports
SOURCES += \
    maintest.cpp \
    comment.cpp \
    post.cpp \
    weblogdatabase.cpp

HEADERS += \
    maintest.h \
    comment.h \
    post.h \
    weblogdatabase.h
