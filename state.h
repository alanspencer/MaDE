#ifndef STATE_H
#define STATE_H

#include <QtGui>

class State
{
public:
    State(QString symbol, QString name, QString notes);

    QString stateSymbol;
    QString stateName;
    QString stateNotes;

    void setSymbol(QString symbol);
    void setName(QString name);
    void setNotes(QString notes);

    QString getSymbol();
    QString getName();
    QString getNotes();
};

#endif // STATE_H
