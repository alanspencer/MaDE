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

#ifndef NXSCHARACTERSBLOCK_H
#define NXSCHARACTERSBLOCK_H

#include <QtWidgets>

class NxsReader;
class NxsBlock;
class NxsException;
class NxsTaxaBlock;
//class NxsAssumptionsBlock;

class NxsCharactersBlock : public NxsBlock
{
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

    NxsCharactersBlock(NxsReader *pointer, NxsTaxaBlock *tBlock);
    virtual ~NxsCharactersBlock();

    bool isEliminated(int origCharIndex);

    virtual QMap<QString,QVariant> getData();
    virtual int charLabelToNumber(QString str);
    virtual int taxonLabelToNumber(QString str);
    virtual void reset();

protected:
    virtual void read(NxsToken &token);

    bool    isInSymbols(QChar ch);

    void    resetSymbols();
    QMap<QString, QString> getDefaultEquates();

    void    handleDimensions(NxsToken &token, QString newtaxaLabel, QString ntaxLabel, QString ncharLabel);
    void    handleFormat(NxsToken &token);
    void    handleEliminate(NxsToken &token);
    void    handleTaxlabels(NxsToken &token);

    void    buildCharPosArray(bool checkEliminated = false);

    NxsTaxaBlock *taxaBlock;    // pointer to the TAXA block in which taxon labels are stored

    int     nchar;              // number of columns in matrix (same as `ncharTotal' unless some characters were eliminated, in which case `ncharTotal' > `nchar')
    int     ncharTotal;         // total number of characters (same as `nchar' unless some characters were eliminated, in which case `ncharTotal' > `nchar')
    QStringList charLabels;     // storage for character labels (if provided)

    int     ntax;               // number of rows in matrix (same as `ntaxTotal' unless fewer taxa appeared in CHARACTERS MATRIX command than were specified in the TAXA block, in which case `ntaxTotal' > `ntax')
    int     ntaxTotal;          // number of taxa (same as `ntax' unless fewer taxa appeared in CHARACTERS MATRIX command than were specified in the TAXA block, in which case `ntaxTotal' > `ntax')

    bool    newtaxa;            // true if NEWTAXA keyword encountered in DIMENSIONS command
    bool    newchar;            // true unless CHARLABELS or CHARSTATELABELS command read

    QChar missing;              // missing data symbol
    QChar gap;                  // gap symbol for use with molecular data
    QChar matchchar;            // match symbol to use in matrix

    bool    tokens;             // if false, data matrix entries must be single symbols; if true, multicharacter entries are allows
    bool    respectingCase;     // if true, RESPECTCASE keyword specified in FORMAT command
    bool    labels;             // indicates whether or not labels will appear on left side of matrix
    bool    transposing;		// indicates matrix will be in transposed format
    bool    interleaving;		// indicates matrix will be in interleaved format

    NxsIntSetMap eliminated;    // array of (0-offset) character numbers that have been eliminated (will remain empty if no ELIMINATE command encountered)
    QList<int> charPos;         // maps character numbers in the data file to column numbers in matrix (necessary if some characters have been eliminated)
    QList<int> taxonPos;        // maps taxon numbers in the data file to row numbers in matrix (necessary if fewer taxa appear in CHARACTERS block MATRIX command than are specified in the TAXA block)

    QMap<int, QStringList> charStates;          // storage for character state labels (if provided)

    bool activeChar;            // `activeChar[i]' true if character `i' not excluded; `i' is in range [0..`nchar')
    bool activeTaxon;           // `activeTaxon[i]' true if taxon `i' not deleted; `i' is in range [0..`ntax')

    QList<QChar> symbols;       // list of valid character state symbols
    QStringList items;

    QMap<QString, QString> equates; // map of symbols to equates (i.e. A = {BCD})

private:
    dataTypesEnum       datatype;			// flag variable (see datatypes enum)
    statesFormatEnum    statesFormat;
};

#endif // NXSCHARACTERSBLOCK_H
