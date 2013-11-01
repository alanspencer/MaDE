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
