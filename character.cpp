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

#include "character.h"

Character::Character(int id, QString name, QString notes)
{
    characterID = id;
    characterEnabled = true;
    characterName = name;
    characterNotes = notes;
    characterOrdered = false;
}

//-- Setters:

void Character::setIsEnabled(bool enabled) {
    characterEnabled = enabled;
}

void Character::setIsOrdered(bool ordered) {
    characterOrdered = ordered;
}

void Character::setName(QString name) {
    characterName = name;
}

void Character::setNotes(QString notes) {
    characterNotes = notes;
}

//-- Getters:

int Character::getID() {
    return characterID;
}

bool Character::getIsEnabled() {
    return characterEnabled;
}

bool Character::getIsOrdered() {
    return characterOrdered;
}

QString Character::getName() {
    return characterName;
}

QString Character::getNotes() {
    return characterNotes;
}

// States - adding and removing
int Character::countStates()
{
    return stateList.count();
}

void Character::addState(QString symbol, QString name, QString notes)
{
    stateList.append(State(symbol, name, notes));
}

void Character::editState(int stateID, QString symbol, QString name, QString notes)
{
    stateList.insert(stateID, State(symbol, name, notes));
}

State Character::getState(int stateID)
{
    return stateList[stateID];
}

void Character::removeState(int stateID)
{
    stateList.removeAt(stateID);
}

