#include "taxon.h"

Taxon::Taxon(int id, QString name, QString notes)
{
    taxonID = id;
    taxonEnabled = true;
    taxonName = name;
    taxonNotes = notes;
}

//-- Setters:

void Taxon::setName(QString name) {
    taxonName = name;
}

void Taxon::setNotes(QString notes) {
    taxonNotes = notes;
}

void Taxon::setIsEnabled(bool enabled) {
    taxonEnabled = enabled;
}


//-- Getters:

int Taxon::getID() {
    return taxonID;
}

bool Taxon::getIsEnabled() {
    return taxonEnabled;
}

QString Taxon::getName() {
    return taxonName;
}

QString Taxon::getNotes() {
    return taxonNotes;
}
