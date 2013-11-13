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

#ifndef NXSBLOCK_H
#define NXSBLOCK_H

class NxsReader;
class NxsToken;
class NxsException;

class NxsBlock
{

public:
    enum NxsCommandResult
    {
        STOP_PARSING_BLOCK,
        HANDLED_COMMAND,
        UNKNOWN_COMMAND
    };

    NxsBlock();
    virtual ~NxsBlock();

    void setNxsReader(NxsReader *pointer);
    void setEnabled();
    void setDisabled();

    QString getID();
    bool getEnabled();
    bool getEmpty();

    virtual void skippingCommand(QString commandName);

    virtual void handleBlockIDCommand(NxsToken &token);
    virtual void handleEndblock(NxsToken &token);
    virtual void handleTitleCommand(NxsToken &token);
    virtual void read(NxsToken &token);
    virtual QMap<QString, QVariant> getData();
    virtual int charLabelToNumber(QString str);
    virtual int taxonLabelToNumber(QString str);
    virtual void reset();

    QString errorMessage;

    NxsBlock *next;		// field pointer to next block in list


protected:
    NxsCommandResult	handleBasicBlockCommands(NxsToken &token);
    void generateUnexpectedTokenException(NxsToken &token, QString expected = NULL);

    void demandEquals(NxsToken &token, QString contextString);
    void demandEndSemicolon(NxsToken &token, QString contextString);
    int demandPositiveInt(NxsToken &token, QString contextString);

    NxsReader *nxs;
    QString blockID;
    bool isEmpty;               // true if this object is not storing data
    bool isEnabled;             // true if this block is currently enabled
    QString title;              // holds the title of the block empty by default
    QString blockIDString;      // Mesquite generates these. Don't know what they are for...
    QMap<QString, QVariant> blockData; // standard data return
};


class NxsBlockFactory
{
public:
    virtual ~NxsBlockFactory(){}
    virtual NxsBlock  *	getBlockReaderForID(const QString & id, NxsReader *) = 0;
    virtual void blockError(NxsBlock *b){ delete b; }
    virtual void blockSkipped(NxsBlock *b) { delete b; }
};

#endif // NXSBLOCK_H