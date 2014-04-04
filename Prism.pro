#-------------------------------------------------
#
# Project created by QtCreator 2013-12-23T16:58:01
#
#-------------------------------------------------

QT       += core gui xml

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
    src/RemoveTrackDialog.cpp \
    src/RenameTrackDialog.cpp \
    src/FileManager.cpp \
    src/CategoryData.cpp \
    src/ExportDialog.cpp \
    src/ImageData.cpp \
    thirdparty/sqlite3/sqlite3.c

HEADERS  += include/ClipItem.h \
    include/TrackItem.h \
    include/ClipParamDialog.h \
    include/MainWindow.h \
    include/ClipModel.h \
    include/TimelineItem.h \
    include/TrackModel.h \
    include/AddTrackDialog.h \
    include/RemoveTrackDialog.h \
    include/RenameTrackDialog.h \
    include/FileManager.h \
    include/CategoryData.h \
    include/ExportDialog.h \
    include/ImageData.h \
    thirdparty/sqlite3/sqlite3.h

FORMS    += forms/MainWindow.ui \
    forms/ClipParamDialog.ui \
    forms/AddTrackDialog.ui \
    forms/RemoveTrackDialog.ui \
    forms/RenameTrackDialog.ui \
    forms/ExportDialog.ui
