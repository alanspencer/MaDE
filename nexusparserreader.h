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

#ifndef NEXUSPARSERREADER_H
#define NEXUSPARSERREADER_H

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
#define NEXUS_MAX_STATES 76

#include <mainwindow.h>
#include <settings.h>

class NexusParserToken;
class NexusParserBlockFactory;
class NexusParserBlock;
class NexusParserException;

typedef QList<NexusParserBlock *> NexusParserBlockList;
typedef QMap<QString, NexusParserBlockList> NexusParserBlockIDToBlockList;

class NexusParserReader
{
public:
    NexusParserReader(MainWindow *mw, Settings *s);

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
    bool execute(NexusParserToken &token);

    bool enteringBlock(QString currentBlockName);
    void exitingBlock(QString currentBlockName);
    void postBlockReadingHook(NexusParserBlock *block);
    void skippingBlock(QString currentBlockName);
    void skippingDisabledBlock(QString currentBlockName);

    void logError(QString message, qint64 filePos, qint64	fileLine, qint64 fileCol);
    void logWarning(QString message, LogWarningLevel warnLevel, NexusParserToken &token);
    void logMesssage(QString message);

    NexusParserBlockIDToBlockList getUsedBlocks();

    enum WarningHandlingMode
    {
        IGNORE_WARNINGS,        // warnings that are not error-generating are ignored silently
        WARNINGS_TO_LOG,        // warnings that are not error-generating are written to application log
        WARNINGS_ARE_ERRORS     // warnings that are not error-generating by some other mechanism are still converted to NexusParserException objects
    };
    WarningHandlingMode currentWarningMode;

    enum LogWarningLevel {
        UNCOMMON_SYNTAX_WARNING = 0,                // Legal but uncommon syntax that could indicate a typo
        SKIPPING_CONTENT_WARNING = 1,               // Content is being skipped by code
        OVERWRITING_CONTENT_WARNING = 2,            // New content is replacing old information (eg. CharSets with the same name as a previously defined CharSet)
        DEPRECATED_WARNING = 3,                     // Use of a deprecated feature
        AMBIGUOUS_CONTENT_WARNING = 4,              // commands that could have multiple plausible interpretations
        ILLEGAL_CONTENT_WARNING = 5,                // content that violates NEXUS rules, but is still parseable (eg. CharPartitions that only have some of the characters)
        PROBABLY_INCORRECT_CONTENT_WARNING = 6,     // Severe Warning that is generated when the file contains characters that should almost certainly be removed
        FATAL_WARNING = 7,                          // a higher warning level then any of the warning generated by code.  Primarily used in constructs such as (FATAL_WARNING - 1) to mean only the most severe warnings.
        SUPPRESS_WARNINGS_LEVEL = 8                 // if the NexusParser's warning level is set to this, then warnings will be suppressed.
    };

protected:
    NexusParserBlock *currentBlock;	/* pointer to current block in list of blocks */
    NexusParserBlock *blockList;


private:
    bool readUntilEndblock(NexusParserToken token, QString currentBlockName);

    NexusParserBlockIDToBlockList blockIDToBlockList;
    void addBlockToUsedBlockList(const QString &, NexusParserBlock *);
    int removeBlockFromUsedBlockList(NexusParserBlock *);

    void loadBlocks();
    QList<QString> blocksToLoad;
};

#endif // NEXUSPARSERREADER_H
