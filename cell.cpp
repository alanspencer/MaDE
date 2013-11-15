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
    isPolymorphic = false;
    isMissing = false;
    isGap = false;
    isMatchchar = false;
    isUncertainty = false;
}

//-- Setters:

void Cell::setState(QString state) {
    cellState = state;
}

void Cell::setPolymorphic(bool polymorphic) {
    isPolymorphic = polymorphic;
}

void Cell::setMissing(bool missing) {
    isMissing = missing;
}

void Cell::setGap(bool gap) {
    isGap = gap;
}

void Cell::setMatchchar(bool matchchar) {
    isMatchchar = matchchar;
}

void Cell::setUncertainty(bool uncertainty) {
    isUncertainty = uncertainty;
}


void Cell::setNotes(QString notes) {
    cellNotes = notes;
}

//-- Getters:

QString Cell::getState() {
    return cellState;
}

bool Cell::getPolymorphic() {
    return isPolymorphic;
}

bool Cell::getMissing() {
    return isMissing;
}

bool Cell::getGap() {
    return isGap;
}

bool Cell::getMatchchar() {
    return isMatchchar;
}

bool Cell::getUncertainty() {
    return isUncertainty;
}

QString Cell::getNotes() {
    return cellNotes;
}
