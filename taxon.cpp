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
