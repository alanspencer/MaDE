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

void State::setName(QString name) {
    stateName = name;
}

void State::setNotes(QString notes) {
    stateNotes = notes;
}

//-- Getters:

QString State::getSymbol() {
    return stateSymbol;
}

QString State::getName() {
    return stateName;
}

QString State::getNotes() {
    return stateNotes;
}
