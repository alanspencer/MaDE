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
