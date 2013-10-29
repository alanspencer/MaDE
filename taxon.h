#ifndef TAXON_H
#define TAXON_H

#include <QtGui>

class Taxon
{
public:
    Taxon(int id, QString name, QString notes);

    void setName(QString name);
    void setNotes(QString notes);
    void setIsEnabled(bool enabled);

    int getID();
    QString getName();
    QString getNotes();
    bool getIsEnabled();

private:
    int taxonID;
    bool taxonEnabled;
    QString taxonName;
    QString taxonNotes;
};

#endif // TAXON_H
