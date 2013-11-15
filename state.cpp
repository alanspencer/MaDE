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

#include "state.h"

State::State(QString symbol, QString name, QString notes)
{
    stateSymbol= symbol;
    stateName = name;
    stateNotes = notes;
}

//-- Setters:

void State::setSymbol(QString symbol) {
    stateSymbol = symbol;
}

void State::setLabel(QString name) {
    stateName = name;
}

void State::setNotes(QString notes) {
    stateNotes = notes;
}

//-- Getters:

QString State::getSymbol() {
    return stateSymbol;
}

QString State::getLabel() {
    return stateName;
}

QString State::getNotes() {
    return stateNotes;
}
