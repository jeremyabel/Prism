#-------------------------------------------------
#
# Project created by QtCreator 2013-12-23T16:58:01
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Prism
TEMPLATE = app


SOURCES += main.cpp\
        MainWindow.cpp \
    TrackItem.cpp \
    TimelineItem.cpp \
    ClipItem.cpp \
    TrackModel.cpp \
    ClipParamDialog.cpp

HEADERS  += MainWindow.h \
    TrackItem.h \
    TimelineItem.h \
    TrackModel.h \
    ClipModel.h \
    ClipItem.h \
    ClipParamDialog.h

FORMS    += MainWindow.ui \
    ClipParamDialog.ui
