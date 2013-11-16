#-----------------------------------------------------------------------------------------------------*/
# Matrix Data Editor (MaDE)
#
# Copyright (c) 2012-2013, Alan R.T. Spencer
#
# This program is free software; you can redistribute it and/or modify it under the terms of the GNU
# General Public License as published by the Free Software Foundation; either version 3 of the License,
# or (at your option) any later version.
#
# This program is free software: you can redistribute it and/or modify it under the terms of the GNU
# General Public License as published by the Free Software Foundation, either version 3 of the License,
# or (at your option) any later version.
#
# You should have received a copy of the GNU General Public License along with this program. If not,
# see http://www.gnu.org/licenses/.
#-----------------------------------------------------------------------------------------------------*/

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
    equate.cpp \
    nexusparserblock.cpp \
    nexusparsercharactersblock.cpp \
    nexusparserexception.cpp \
    nexusparserreader.cpp \
    nexusparsersetreader.cpp \
    nexusparsertaxablock.cpp \
    nexusparsertoken.cpp \
    nexusparserassumptionsblock.cpp

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
    equate.h \
    nexusparserblock.h \
    nexusparsercharactersblock.h \
    nexusparserexception.h \
    nexusparserreader.h \
    nexusparsersetreader.h \
    nexusparsertaxablock.h \
    nexusparsertoken.h \
    nexusparser.h \
    nexusparserassumptionsblock.h

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

OTHER_FILES += \
    README.md
