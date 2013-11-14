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

#ifndef NXSREADER_H
#define NXSREADER_H

/*------------------------------------------------------------------------------------/
 * Single load point for all NEXUS parsing
 *
 * Set all variables needed for all other classes/headers, then include classes/headers
 *-----------------------------------------------------------------------------------*/

// Maximum number of states that can be stored; the only limitation is that this
// number be less than the maximum size of an int (not likely to be a problem).
// A good number for this is 76, which is 96 (the number of distinct symbols
// able to be input from a standard keyboard) less 20 (the number of symbols
// symbols disallowed by the NEXUS standard for use as state symbols)
#define NXS_MAX_STATES 76

#include <mainwindow.h>
#include <settings.h>

class NxsToken;
class NxsBlockFactory;
class NxsBlock;
class NxsException;

typedef QList<NxsBlock *> NxsBlockList;
typedef QMap<QString, NxsBlockList> NxsBlockIDToBlockList;

class NxsReader
{
public:
    NxsReader(MainWindow *mw, Settings *s);

    MainWindow *mainwindow;
    Settings *settings;

    QStringList defaultStandardStateSet;
    QStringList allowedStandardStateSet;
    QStringList defaultDNAStateSet;
    QStringList defaultRNAStateSet;
    QStringList defaultNucleotideStateSet;
    QStringList defaultProteinStateSet;

    QStringList defaultDNAEquateStates;
    QStringList defaultRNAEquateStates;
    QStringList defaultNucleotideEquateStates;
    QStringList defaultProteinEquateStates;

    QChar defaultGapCharacter;
    QChar defaultMissingCharacter;
    QChar defaultMatchCharacter;

    void addBlock(QString blockID);
    int getBlockCount(QString blockID);
    bool execute(NxsToken &token);

    bool enteringBlock(QString currentBlockName);
    void exitingBlock(QString currentBlockName);
    void postBlockReadingHook(NxsBlock *block);
    void skippingBlock(QString currentBlockName);
    void skippingDisabledBlock(QString currentBlockName);

    void NxsLogError(QString message, qint64 filePos, qint64	fileLine, qint64 fileCol);
    void NxsLogMesssage(QString message);

    NxsBlockIDToBlockList getUsedBlocks();

protected:
    NxsBlock *currentBlock;	/* pointer to current block in list of blocks */
    NxsBlock *blockList;


private:
    bool readUntilEndblock(NxsToken token, QString currentBlockName);

    NxsBlockIDToBlockList blockIDToBlockList;
    void addBlockToUsedBlockList(const QString &, NxsBlock *);
    int removeBlockFromUsedBlockList(NxsBlock *);

    void loadBlocks();
    QList<QString> blocksToLoad;
};

#endif // NXSREADER_H
