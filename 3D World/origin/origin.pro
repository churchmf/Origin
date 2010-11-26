QT += opengl

TARGET = origin
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app
INCLUDEPATH += .


# Input
HEADERS += originwindow.h \
    mainwindow.h
SOURCES += main.cpp \
    mainwindow.cpp \
    originwindow.cpp \
    inputhandler.cpp
