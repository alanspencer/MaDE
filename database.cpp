#include "database.h"

Database::Database()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(settings->getSetting("defaultDatabaseName").toString());
    if (db.open()) {
        qDebug() << "SQLite: Database found and opened.";
        /* Check if there is a SQLite DB available. If yes then connect,
         * if not then ask if user wants to load a db file or create a new
         * db
         */
        QSqlQuery query(db);
        if ( !query.exec("SELECT * FROM settings;") ) {
            // No settings table found...
            this->createSchema();
        }
    } else {
        qDebug() << "SQLite: Error - No Database File found.";
    }
}

// Create Database Tables
void Database::createSchema()
{
    QSqlQuery query(db);
    // No database tables found... assume we need to create.
    query.exec(
        "CREATE TABLE settings("
        "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT NOT NULL,"
        "value TEXT NOT NULL"
        ");"
    );
    query.exec(
        "CREATE TABLE matrices("
        "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
        "prefix TEXT NOT NULL,"
        "name TEXT NOT NULL,"
        "description TEXT NOT NULL,"
        "creationDate DATETIME NOT NULL,"
        "lastEditDate DATETIME,"
        "notes TEXT"
        ");"
    );
    qDebug() << "SQLite: Created a new database schema.";

    // Set default settings
    this->setSetting("appName", "Matrix Data Editor (MaDE)");
    this->setSetting("appVersion", QString::number(APP_VERSION, 'f', 1) );
    QDateTime currentDate = QDateTime::currentDateTime();
    this->setSetting("creationDate", currentDate.toString("yyyy-MM-dd hh:mm:ss") );
}

// Create Settings
void Database::setSetting(QString name, QString value) {
    QSqlQuery query(db);
    query.prepare("INSERT INTO settings(name,value) VALUES (:name,:value);");
    query.bindValue(":name", name);
    query.bindValue(":value", value);
    if ( !query.exec() )
    {
        qDebug() << "SQLite: Error -" << query.lastError();
    }
}

// Get Settings
QString Database::getSetting(QString name) {
    QString value;
    QSqlQuery query(db);
    query.prepare(
        "SELECT value FROM settings WHERE name=:name;"
    );
    query.bindValue(":name", name);
    query.exec();
    query.next();
    value = query.value(0).toString();
    return value;
}

// Create New Matrix
void Database::newMatrix(){
    QSqlQuery query(db);
    QString tablePrefix;
    int defaultNumTaxa;
    int defaultNumCharacter;

    // Add entry into matrices table


    // Create matrix table set
    query.exec(
        "CREATE TABLE "+tablePrefix+"_data("
        "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
        "TID INTEGER NOT NULL,"
        "CID INTEGER NOT NULL,"
        "state TEXT,"
        "notes TEXT,"
        ");"
    );
    query.exec(
        "CREATE TABLE "+tablePrefix+"_taxa("
        "TID INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT NOT NULL,"
        "notes TEXT"
        ");"
    );
    query.exec(
        "CREATE TABLE "+tablePrefix+"_characters("
        "CID INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT NOT NULL,"
        "notes TEXT"
        ");"
    );
    query.exec(
        "CREATE TABLE "+tablePrefix+"_states("
        "SID INTEGER PRIMARY KEY AUTOINCREMENT,"
        "CID INTEGER NOT NULL,"
        "state INTEGER NOT NULL,"
        "name TEXT NOT NULL,"
        "notes TEXT"
        ");"
    );

}
