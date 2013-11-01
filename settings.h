/*------------------------------------------------------------------------------------------------------
 * Matrix Data Editor (MaDE)
 *
 * Copyright (c) 2012-2013, Alan R.T. Spencer
 *
 * This program is free software; you can redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software Foundation; either version 3 of the License,
 * or (at your option) any later version.
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software Foundation, either version 3 of the License,
 * or (at your option) any later version.
 *
 * You should have received a copy of the GNU General Public License along with this program. If not,
 * see http://www.gnu.org/licenses/.
 *-----------------------------------------------------------------------------------------------------*/

#ifndef SETTINGS_H
#define SETTINGS_H

#include "mainwindow.h"

#include <QDebug>

class MainWindow;

class Settings
{
public:   
    Settings();
    void initialize();

    MainWindow *mw;

    void setDefaultSettings();
    void setSetting(QString key, QVariant value);
    QVariant getSetting(QString key);

private:
    bool isFileEmpty();

    QHash<QString, QVariant> defaultSettingsList;
};

#endif // SETTINGS_H
