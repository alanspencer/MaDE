#ifndef EQUATE_H
#define EQUATE_H

#include <QString>

class Equate
{
public:
    Equate(int id, QString symbol, QString equivalent);

    void setSymbol(QString symbol);
    void setEquivalent(QString equivalent);
    void setIsEnabled(bool enabled);

    int getID();
    QString getSymbol();
    QString getEquivalent();
    bool getIsEnabled();

private:
    int equateID;               // unique id for each equate pairing
    bool equateEnabled;         // bool value may not ever be required but here just in case!
    QString equatesymbol;       // a single character e.g. 'z'
    QString equateEquivalent;   // multiple characters should be in the form '{abc}'
};

#endif // EQUATE_H
