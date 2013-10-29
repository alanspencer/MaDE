#ifndef CELL_H
#define CELL_H

#include <QtGui>

class Cell
{
public:
    Cell(QString state, QString notes);

    QString cellState;
    QString cellNotes;

    void setState(QString state);
    void setNotes(QString notes);
    QString getState();
    QString getNotes();
};

#endif // CELL_H
