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

#ifndef NEXUSPARSERBLOCK_H
#define NEXUSPARSERBLOCK_H

class NexusParserReader;
class NexusParserToken;
class NexusParserException;

class NexusParserBlock
{

public:
    enum NexusParserCommandResult
    {
        STOP_PARSING_BLOCK,
        HANDLED_COMMAND,
        UNKNOWN_COMMAND
    };

    NexusParserBlock();
    virtual ~NexusParserBlock();

    void setNexusParserReader(NexusParserReader *pointer);
    void setEnabled();
    void setDisabled();

    QString getID();
    bool getEnabled();
    bool getEmpty();

    virtual void skippingCommand(QString commandName);

    virtual void handleBlockIDCommand(NexusParserToken &token);
    virtual void handleEndblock(NexusParserToken &token);
    virtual void handleTitleCommand(NexusParserToken &token);
    virtual void read(NexusParserToken &token);
    virtual int charLabelToNumber(QString str);
    virtual int taxonLabelToNumber(QString str);
    virtual void reset();

    QString errorMessage;

    NexusParserBlock *next;		// field pointer to next block in list


protected:
    NexusParserCommandResult	handleBasicBlockCommands(NexusParserToken &token);
    void generateUnexpectedTokenException(NexusParserToken &token, QString expected = NULL);

    void demandEquals(NexusParserToken &token, QString contextString);
    void demandEndSemicolon(NexusParserToken &token, QString contextString);
    int demandPositiveInt(NexusParserToken &token, QString contextString);

    NexusParserReader *nexusParser;
    QString blockID;
    bool isEmpty;               // true if this object is not storing data
    bool isEnabled;             // true if this block is currently enabled
    QString title;              // holds the title of the block empty by default
    QString blockIDString;      // Mesquite generates these. Don't know what they are for...
    QMap<QString, QVariant> blockData; // standard data return
};


class NexusParserBlockFactory
{
public:
    virtual ~NexusParserBlockFactory(){}
    virtual NexusParserBlock  *	getBlockReaderForID(const QString & id, NexusParserReader *) = 0;
    virtual void blockError(NexusParserBlock *b){ delete b; }
    virtual void blockSkipped(NexusParserBlock *b) { delete b; }
};

#endif // NEXUSPARSERBLOCK_H
