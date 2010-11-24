#-------------------------------------------------
#
# Project created by QtCreator 2010-11-23T20:27:37
#
#-------------------------------------------------

QT       += opengl

TARGET = 3DWorld
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    cswindow.cpp \
    cswindow_helper.cpp

HEADERS  += mainwindow.h \
    cswindow.h

FORMS    += mainwindow.ui
