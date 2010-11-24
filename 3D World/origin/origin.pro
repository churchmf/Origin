QT += opengl
TEMPLATE = app
INCLUDEPATH += .
CONFIG   += console

# Input
HEADERS += originwindow.h \
    mainwindow.h
SOURCES += main.cpp \
    mainwindow.cpp \
    originwindow.cpp \
    inputhandler.cpp

FORMS += \
    mainwindow.ui
