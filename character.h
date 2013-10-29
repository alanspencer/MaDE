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
    QString characterName;
    QString characterNotes;
    QList <State> stateList;

    bool characterOrdered;

    void setIsEnabled(bool enabled);
    void setIsOrdered(bool ordered);
    void setName(QString name);
    void setNotes(QString notes);

    int getID();
    bool getIsEnabled();
    bool getIsOrdered();
    QString getName();
    QString getNotes();

    int countStates();
    void addState(QString symbol, QString name, QString notes);
    void editState(int stateID, QString symbol, QString name, QString notes);
    State getState(int stateID);
    void removeState(int stateID);
};

#endif // CHARACTER_H
