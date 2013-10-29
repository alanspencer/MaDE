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
