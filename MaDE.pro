#-------------------------------------------------
#
# Project created by QtCreator 2013-09-17T16:13:54
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MaDE
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    settings.cpp \
    database.cpp \
    taxon.cpp \
    character.cpp \
    state.cpp \
    cell.cpp \
    matrix.cpp \
    settingsdialog.cpp \
    matrixsettingsdialog.cpp \
    taxadialog.cpp \
    charactersdialog.cpp \
    nexusreaderexception.cpp \
    nexusreadertoken.cpp \
    nexusreader.cpp

HEADERS  += mainwindow.h \
    settings.h \
    database.h \
    taxon.h \
    character.h \
    state.h \
    cell.h \
    matrix.h \
    settingsdialog.h \
    matrixsettingsdialog.h \
    taxadialog.h \
    charactersdialog.h \
    nexusreader.h \
    nexusreaderexception.h \
    nexusreadertoken.h

FORMS    += mainwindow.ui \
    matrixTable.ui \
    settingsDialog.ui \
    matrixsettingsdialog.ui \
    taxadialog.ui \
    charactersdialog.ui

# The application version
VERSION = 0.1

# Define the preprocessor macro to get the application version in our application.
DEFINES += APP_VERSION=$$VERSION

RESOURCES += \
    resources.qrc
