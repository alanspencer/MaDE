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

#ifndef STATE_H
#define STATE_H

#include <QtGui>

class State
{
public:
    State(QString symbol, QString name, QString notes);

    QString stateSymbol;
    QString stateName;
    QString stateNotes;

    void setSymbol(QString symbol);
    void setLabel(QString name);
    void setNotes(QString notes);

    QString getSymbol();
    QString getLabel();
    QString getNotes();
};

#endif // STATE_H
