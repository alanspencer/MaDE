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

#ifndef NEXUSREADERBLOCK_H
#define NEXUSREADERBLOCK_H

#include <QWidget>

#include "nexusreader.h"

class NexusReader;
class NexusReaderToken;

class NexusReaderBlock
{

friend class NexusReader;

public:
    enum NexusCommandResult
    {
        STOP_PARSING_BLOCK,
        HANDLED_COMMAND,
        UNKNOWN_COMMAND
    };

    NexusReaderBlock();
    virtual ~NexusReaderBlock();

    void setNexusReader(NexusReader *pointer);
    void setEnabled();
    void setDisabled();

    QString getID();
    bool getEnabled();
    bool getEmpty();

    virtual void skippingCommand(QString commandName);

    virtual void handleBlockIDCommand(NexusReaderToken *&token);
    virtual void handleEndblock(NexusReaderToken *&token);
    virtual void handleTitleCommand(NexusReaderToken *&token);

    virtual void reset();

    QString errorMessage;

protected:
    NexusCommandResult	handleBasicBlockCommands(NexusReaderToken *&token);
    void generateUnexpectedTokenException(NexusReaderToken *&token, QString expected = NULL);

    virtual void read(NexusReaderToken *&token);
    void requireEqualsToken(NexusReaderToken *&token, QString contextString);
    void requireSemicolonToken(NexusReaderToken *&token, QString contextString);
    int requirePositiveToken(NexusReaderToken *&token, QString contextString);

    NexusReader *nexusReader;
    QString blockID;
    bool isEmpty;               // true if this object is not storing data
    bool isEnabled;             // true if this block is currently enabled
    QString title;              // holds the title of the block empty by default
    QString blockIDString;      // Mesquite generates these. Don't know what they are for...
    NexusReaderBlock *next;		// field pointer to next block in list

};


class NexusReaderBlockFactory
{
public:
    virtual ~NexusReaderBlockFactory(){}
    virtual NexusReaderBlock  *	getBlockReaderForID(const QString & id, NexusReader *) = 0;
    virtual void blockError(NexusReaderBlock *b){ delete b; }
    virtual void blockSkipped(NexusReaderBlock *b) { delete b; }
};

#endif // NEXUSREADERBLOCK_H
