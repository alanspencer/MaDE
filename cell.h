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

#ifndef CELL_H
#define CELL_H

#include <QtGui>

class Cell
{
public:
    Cell(QString state, QString notes);

    QString cellState;
    QString cellNotes;
    bool isPolymorphic;
    bool isMissing;
    bool isGap;
    bool isMatchchar;
    bool isUncertainty;

    void setState(QString state);
    void setPolymorphic(bool polymorphic);
    void setMissing(bool missing);
    void setGap(bool gap);
    void setMatchchar(bool matchchar);
    void setUncertainty(bool uncertainty);
    void setNotes(QString notes);

    QString getState();
    bool getPolymorphic();
    bool getMissing();
    bool getGap();
    bool getMatchchar();
    bool getUncertainty();
    QString getNotes();
};

#endif // CELL_H
