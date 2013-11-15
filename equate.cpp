#include "Equate.h"

Equate::Equate(int id, QString symbol, QString equivalent)
{
    equateID = id;
    equateEnabled = true;
    equatesymbol = symbol;
    equateEquivalent = equivalent;
}

//-- Setters:

void Equate::setSymbol(QString symbol) {
    equatesymbol = symbol;
}

void Equate::setEquivalent(QString equivalent) {
    equateEquivalent = equivalent;
}

void Equate::setIsEnabled(bool enabled) {
    equateEnabled = enabled;
}


//-- Getters:

int Equate::getID() {
    return equateID;
}

bool Equate::getIsEnabled() {
    return equateEnabled;
}

QString Equate::getSymbol() {
    return equatesymbol;
}

QString Equate::getEquivalent() {
    return equateEquivalent;
}
