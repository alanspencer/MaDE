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

#include "cell.h"

Cell::Cell(QString state, QString notes)
{
    cellState = state;
    cellNotes = notes;
}

//-- Setters:

void Cell::setState(QString state) {
    this->cellState = state;
}

void Cell::setNotes(QString notes) {
    this->cellNotes = notes;
}

//-- Getters:

QString Cell::getState() {
    return this->cellState;
}

QString Cell::getNotes() {
    return this->cellNotes;
}
