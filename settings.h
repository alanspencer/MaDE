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
