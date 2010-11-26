QT += opengl

TARGET = origin
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app
INCLUDEPATH += .


# Input
HEADERS += originwindow.h \
    mainwindow.h \
    scene.h \
    3dobject.h

SOURCES += main.cpp \
    mainwindow.cpp \
    originwindow.cpp \
    inputhandler.cpp \
    3dobject.cpp
