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

