#-------------------------------------------------
#
# Project created by QtCreator 2013-12-23T16:58:01
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Prism
TEMPLATE = app
INCLUDEPATH += src/core \
	       src/commands \
	       src/data \
	       src/dialogs \
	       src/graphics \
	       src/models \

SOURCES +=  src/main.cpp \
	    src/graphics/ClipItem.cpp \
	    src/core/MainWindow.cpp \
	    src/graphics/TrackItem.cpp \
	    src/dialogs/ClipParamDialog.cpp \
	    src/graphics/TimelineItem.cpp \
	    src/models/TrackModel.cpp \
	    src/dialogs/AddTrackDialog.cpp \
	    src/core/FileManager.cpp \
	    src/data/CategoryData.cpp \
	    src/dialogs/ExportDialog.cpp \
	    src/data/ImageData.cpp \
	    thirdparty/sqlite3/sqlite3.c \
	    src/models/ClipModel.cpp \
	    src/commands/TrackCommands.cpp \
	    src/commands/ClipCommands.cpp

HEADERS  += src/graphics/ClipItem.h \
	    src/graphics/TrackItem.h \
	    src/dialogs/ClipParamDialog.h \
	    src/core/MainWindow.h \
	    src/models/ClipModel.h \
	    src/graphics/TimelineItem.h \
	    src/models/TrackModel.h \
	    src/dialogs/AddTrackDialog.h \
	    src/core/FileManager.h \
	    src/data/CategoryData.h \
	    src/dialogs/ExportDialog.h \
	    src/data/ImageData.h \
	    thirdparty/sqlite3/sqlite3.h \
	    src/commands/TrackCommands.h \
	    src/commands/ClipCommands.h

FORMS    += forms/MainWindow.ui \
	    forms/ClipParamDialog.ui \
	    forms/AddTrackDialog.ui \
	    forms/ExportDialog.ui

unix:!macx{
LIBS += -ldl
}
