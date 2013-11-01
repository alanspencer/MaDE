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

#ifndef DATABASE_H
#define DATABASE_H

#include <QDebug>

#include <QtSql/QtSql>
#include <QDateTime>

#include <settings.h>

class Settings;

class Database
{
public:
    Database();
    void setSetting(QString name, QString value);
    QString getSetting(QString name);

    void newMatrix();

private:
    QSqlDatabase db;
    Settings *settings;
    void createSchema();
};

#endif // DATABASE_H
