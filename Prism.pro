#-------------------------------------------------
#
# Project created by QtCreator 2013-12-23T16:58:01
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Prism
TEMPLATE = app
INCLUDEPATH += include

SOURCES += src/main.cpp\
    src/ClipItem.cpp \
    src/MainWindow.cpp \
    src/TrackItem.cpp \
    src/ClipParamDialog.cpp \
    src/TimelineItem.cpp \
    src/TrackModel.cpp \
    src/AddTrackDialog.cpp \

HEADERS  += include/ClipItem.h \
    include/TrackItem.h \
    include/ClipParamDialog.h \
    include/MainWindow.h \
    include/ClipModel.h \
    include/TimelineItem.h \
    include/TrackModel.h \
    include/AddTrackDialog.h \

FORMS    += forms/MainWindow.ui \
    forms/ClipParamDialog.ui \
    forms/AddTrackDialog.ui \
