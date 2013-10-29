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
