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

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include "ui_settingsdialog.h"

#include <QtGui>
#include <QWidget>

#include <mainwindow.h>
#include <settings.h>

class MainWindow;
class Settings;

class SettingsDialog : public QDialog, Ui::settingsDialog
{
    Q_OBJECT

public:
    SettingsDialog(QWidget *parent = 0);

    void initalize();

    MainWindow *mw;
    Settings *settings;

private:
    void loadValues();

private slots:
    void buttonActions(QAbstractButton *button);
    void actionRestoreDefaults();
    void actionSave();
    void actionClose();
};

#endif // SETTINGSDIALOG_H
