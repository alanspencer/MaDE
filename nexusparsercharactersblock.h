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

/*------------------------------------------------------------------------------------------------------
 * Modified from work by Paul O. Lewis and Mark T. Holder. Available from http://sourceforge.net/p/ncl/
 * under a GNU General Public License version 2.0 (GPLv2) Licence.
 *
 * Nexus Class Libray (NCL)
 *
 * Copyright (c) 2003-2012, Paul O. Lewis
 * Copyright (c) 2007-2012, Mark T. Holder
 *
 * This program is free software; you can redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program; if not,
 * write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
 * USA.
 *-----------------------------------------------------------------------------------------------------*/

#ifndef NEXUSPARSERCHARACTERSBLOCK_H
#define NEXUSPARSERCHARACTERSBLOCK_H

#include <QtWidgets>

class NexusParserReader;
class NexusParserToken;
class NexusParserBlock;
class NexusParserException;
class NexusParserTaxaBlock;
class Character;
class Equate;
class Cell;
//class NexusParserAssumptionsBlock;

class NexusParserCharactersBlock : public NexusParserBlock
{

//typedef QHash<QPair<int, int>, Cell*> NexusParserCharMatrix;

public:
    enum dataTypesEnum		// values used to represent different basic types of data stored in a CHARACTERS block, and used with the data member `datatype'
    {
        standard = 1,		// indicates `matrix' holds characters with arbitrarily-assigned, discrete states, such as discrete morphological data
        dna,				// indicates `matrix' holds DNA sequences (states A, C, G, T)
        rna,				// indicates `matrix' holds RNA sequences (states A, C, G, U)
        nucleotide,			// indicates `matrix' holds nucleotide sequences
        protein,			// indicates `matrix' holds amino acid sequences
        continuous			// indicates `matrix' holds continuous data
    };

    enum statesFormatEnum
    {
        STATES_PRESENT = 1,
        STATE_COUNT,
        STATE_FREQUENCY,
        INDIVIDUALS
    };

    NexusParserCharactersBlock(NexusParserReader *pointer, NexusParserTaxaBlock *tBlock);
    virtual ~NexusParserCharactersBlock();

    bool isEliminated(int origCharIndex);

    virtual int charLabelToNumber(QString str);
    virtual int taxonLabelToNumber(QString str);
    int getCharPos(int origCharIndex);
    virtual void reset();

protected:
    virtual void read(NexusParserToken &token);

    void    handleDimensions(NexusParserToken &token, QString newtaxaLabel, QString ntaxLabel, QString ncharLabel);
    void    handleFormat(NexusParserToken &token);
    void    handleEliminate(NexusParserToken &token);
    void    handleTaxlabels(NexusParserToken &token);
    void    handleCharstatelabels(NexusParserToken &token);
    void    handleCharlabels(NexusParserToken &token);
    void    handleStatelabels(NexusParserToken &token);
    void    handleMatrix(NexusParserToken &token);
    void    handleStandardMatrix(NexusParserToken &token);
    void    handleTransposedMatrix(NexusParserToken &token);
    bool    handleNextState(NexusParserToken &token, int currentTaxon, int currentCharacter);

    bool    isInSymbols(QChar ch);
    void    resetSymbols();
    void    buildCharPosArray(bool checkEliminated = false);

    NexusParserTaxaBlock *taxaBlock;                    // pointer to the TAXA block in which taxon labels are stored

    int nextCharacterID;                        // int to give each character a unique ID
    int characterAdd(QString characterLabel, bool isEliminated = false);    // Add a new character to the QList<Character> characterList.
    void characterLabelEdit(int position, QString characterLabel);          // Edits the character label stored to the QList<Character> characterList at the given characterID.
    QList<Character> characterList;             // storage for character and state data
    int     nchar;                              // number of columns in matrix (same as `ncharTotal' unless some characters were eliminated, in which case `ncharTotal' > `nchar')
    int     ncharTotal;                         // total number of characters (same as `nchar' unless some characters were eliminated, in which case `ncharTotal' > `nchar')
    bool    newchar;                            // true unless CHARLABELS or CHARSTATELABELS command read

    int     ntax;                               // number of rows in matrix (same as `ntaxTotal' unless fewer taxa appeared in CHARACTERS MATRIX command than were specified in the TAXA block, in which case `ntaxTotal' > `ntax')
    int     ntaxTotal;                          // number of taxa (same as `ntax' unless fewer taxa appeared in CHARACTERS MATRIX command than were specified in the TAXA block, in which case `ntaxTotal' > `ntax')
    bool    newtaxa;                            // true if NEWTAXA keyword encountered in DIMENSIONS command

    QChar   missing;                            // missing data symbol
    QChar   gap;                                // gap symbol for use with molecular data
    QChar   matchchar;                          // match symbol to use in matrix
    QList<QChar> symbols;                       // list of valid character state symbols

    QHash<QPair<int, int>, Cell*> matrixGrid;   // stores the matrix data
    bool cellAdd(                               // add a new cell to the matrix data
            int taxonID,
            int characterID,
            QString state,
            QString notes = "",
            bool isPolymorphic = false,
            bool isMissing = false,
            bool isGap = false,
            bool isMatchchar = false,
            bool isUncertain = false);
    int cellCount();                            // return a count of the cells in matrixGrid
    QPair<int,int> returnLocator(int taxonID, int characterID);             // returns a locator baseded on the taxonID and characterID

    int nextEquatesID;                          // int to give each equate a unique ID
    QList<Equate> equatesList;                  // list of equate symbols (i.e. A = {BCD})
    int equateAdd(QString symbol, QString equivalent);  // Add an equate symbol/equivalent pair to the QList<Equate> equatesList.
    QList<Equate> setDefaultEquates();          // Set the default equates as defined in settings.ini

    bool    tokens;                             // if false, data matrix entries must be single symbols; if true, multicharacter entries are allows
    bool    respectingCase;                     // if true, RESPECTCASE keyword specified in FORMAT command
    bool    labels;                             // indicates whether or not labels will appear on left side of matrix
    bool    transposing;                        // indicates matrix will be in transposed format
    bool    interleaving;                       // indicates matrix will be in interleaved format

    QMap<int, int> eliminated;                  // map of (0-offset) character numbers that have been eliminated (== disabled) ordered by key which is the same as the value. Dirty version of a std::set. Will remain empty if no ELIMINATE command encountered.
    QMap<int, int> charPos;                     // maps character numbers in the data file to column numbers in matrix (key = character pos; value = new character pos; necessary if some characters have been eliminated)
    QMap<int, int> taxonPos;                    // maps taxon numbers in the data file to row numbers in matrix (necessary if fewer taxa appear in CHARACTERS block MATRIX command than are specified in the TAXA block)
    QMap<int, bool> activeChar;                 // `activeChar[i]' true if character `i' not excluded; `i' is in range [0..`nchar')
    QMap<int, bool> activeTaxon;                // `activeTaxon[i]' true if taxon `i' not deleted; `i' is in range [0..`ntax')

    QStringList items;                          // list of items

private:
    dataTypesEnum       datatype;       // flag variable (see datatypes enum)
    statesFormatEnum    statesFormat;   // flag variable (see statesFormat enum)
};

#endif // NEXUSPARSERCHARACTERSBLOCK_H
