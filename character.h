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

#ifndef CHARACTER_H
#define CHARACTER_H

#include <QtGui>

#include <state.h>

class Character
{
public:
    Character(int id, QString name, QString notes);

    int characterID;
    bool characterEnabled;
    bool characterEliminated;
    QString characterName;
    QString characterNotes;
    QList <State> stateList;

    bool characterOrdered;

    void setIsEnabled(bool enabled);
    void setIsEliminated(bool eliminate);
    void setIsOrdered(bool ordered);
    void setLabel(QString name);
    void setNotes(QString notes);

    int getID();
    bool getIsEnabled();
    bool getIsEliminated();
    bool getIsOrdered();
    QString getLabel();
    QString getNotes();

    int countStates();
    void addState(QString symbol, QString name, QString notes);
    void editState(int stateID, QString symbol, QString name, QString notes);
    State getState(int stateID);
    void removeState(int stateID);
};

#endif // CHARACTER_H
