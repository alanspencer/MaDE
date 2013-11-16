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

#include "nexusparser.h"

NexusParserBlock::NexusParserBlock()
{
    next = NULL;
    nexusParser = NULL;
    isEmpty = true;
    isEnabled = true;
    blockID.clear();
    errorMessage.clear();
    title = "";
    blockIDString = "";
}

// Nothing to be done.
NexusParserBlock::~NexusParserBlock()
{

}

// This virtual function must be overridden for each derived class to provide the ability to read everything following
// the block name (which is read by the Nxs object) to the end or endblock statement. Characters are read from
// the input stream 'in'. Note that to get output comments displayed, you must derive a class from NexusParserToken,
// override the member function OutputComment to display a supplied comment, and then pass a reference to an object of
// the derived class to this function.
void NexusParserBlock::read(NexusParserToken &)
{

}

// Hook to consolidate the handling of COMMANDS that are common to all blocks (TITLE, BLOCKID, END, ENDBLOCK -- and,
// evenually, LINK).
// HandleXYZ() where XYZ is the command name is then called.
// Returns NexusParserCommandResult(HANDLED_COMMAND), NexusParserCommandResult(HANDLED_COMMAND), or NexusParserCommandResult(UNKNOWN_COMMAND)
// to tell the caller whether the command was recognized.
NexusParserBlock::NexusParserCommandResult NexusParserBlock::handleBasicBlockCommands(NexusParserToken &token)
{
    if (token.equals("TITLE")){
        handleTitleCommand(token);
        return NexusParserBlock::NexusParserCommandResult(HANDLED_COMMAND);
    }
    if (token.equals("BLOCKID")){
        handleBlockIDCommand(token);
        return NexusParserBlock::NexusParserCommandResult(HANDLED_COMMAND);
    }
    if (token.equals("END") || token.equals("ENDBLOCK")){
        handleEndblock(token);
        return NexusParserBlock::NexusParserCommandResult(STOP_PARSING_BLOCK);
    }
    return NexusParserBlock::NexusParserCommandResult(UNKNOWN_COMMAND);
}

// Stores the next token as the this->title field.
void NexusParserBlock::handleTitleCommand(NexusParserToken &token)
    {
    token.getNextToken();
    if (token.equals(";")) {
        generateUnexpectedTokenException(token, "a title for the block");
    }
    title = token.getToken();
    demandEndSemicolon(token, "TITLE");
}

// Stores the next token as the this->blockid field.
void NexusParserBlock::handleBlockIDCommand(NexusParserToken &token)
{
    token.getNextToken();
    if (token.equals(";")) {
        generateUnexpectedTokenException(token, "an id for the block");
    }
    blockIDString = token.getToken();
    demandEndSemicolon(token, "BLOCKID");
}

// Called when the END or ENDBLOCK command needs to be parsed from within a block.
// Basically just checks to make sure the next token in the data file is a semicolon.
void NexusParserBlock::handleEndblock(NexusParserToken &token)
{
    demandEndSemicolon(token, "END or ENDBLOCK");
}

// throws a NexusParserException with the token info for `token`
// `expected` should fill in the phrase "Expecting ${expected}, but found..."
// expected can be NULL. Sets this->errorMessage
void NexusParserBlock::generateUnexpectedTokenException(NexusParserToken &token, QString expected)
{
    errorMessage = "Unexpected token";
    if (!expected.isEmpty()){
        errorMessage += ". Expecting ";
        errorMessage += expected;
        errorMessage += ", but found: ";
    } else {
        errorMessage += ": ";
    }
    errorMessage += token.getToken();
    throw NexusParserException(errorMessage, token.getFilePosition(), token.getFileLine(), token.getFileColumn());
}


// Returns the id QString
QString NexusParserBlock::getID()
{
    return blockID;
}

void NexusParserBlock::setNexusParserReader(NexusParserReader *pointer)
{
    nexusParser = pointer;
}

// Sets the value of isEnabled to true. A NexusParserBlock can be disabled (by calling setDisable) if blocks of that type are to
// be skipped during execution of the NEXUS file. If a disabled block is encountered, the virtual
// Nxs::skippingDisabledBlock function is called, giving your application the opportunity to inform the user
// that a block was skipped.
void NexusParserBlock::setEnabled()
{
    isEnabled = true;
}

// Sets the value of isEnabled to false. A NexusParserBlock can be disabled (by calling this method) if blocks of that type
// are to be skipped during execution of the NEXUS file. If a disabled block is encountered, the virtual
// Nxs::skippingDisabledBlock function is called, giving your application the opportunity to inform the user
// that a block was skipped.
void NexusParserBlock::setDisabled()
{
    isEnabled = false;
}

// Returns value of isEnabled, which can be controlled through use of the Enable and Disable member functions. A
// NexusParserBlock should be disabled if blocks of that type are to be skipped during execution of the NEXUS file. If a
// disabled block is encountered, the virtual Nxs::skippingDisabledBlock function is called, giving your
// application the opportunity to inform the user that a block was skipped.
bool NexusParserBlock::getEnabled()
{
    return isEnabled;
}

// Returns true if Read function has not been called since the last Reset. This base class version simply returns the
// value of the data member isEmpty. If you derive a new block class from NexusParserBlock, be sure to set isEmpty to true in
// your Reset function and isEmpty to false in your Read function.
bool NexusParserBlock::getEmpty()
{
    return isEmpty;
}


// This virtual function should be overridden for each derived class to completely reset the block object in
// preparation for reading in another block of this type. This function is called by the Nxs object just prior to
// calling the block object's Read function.
void NexusParserBlock::reset()
{
    title = "";
    errorMessage.clear();
    isEnabled = true;
    isEmpty = true;
}

// This function is called when an unknown command named commandName is about to be skipped. This version of the
// function does nothing (i.e., no warning is issued that a command was unrecognized). Override this virtual function
// in a derived class to provide such warnings to the user.
void NexusParserBlock::skippingCommand(QString){}

// Advances the token, and raise an exception if it is not an equals sign. Sets errormsg and raises a
// NexusParserException on failure.
void NexusParserBlock::demandEquals(NexusParserToken &token, QString contextString)
{
    token.getNextToken();
    if (!token.equals("=")){
        errorMessage = "Expecting '=' ";
        if (!contextString.isEmpty()) {
            errorMessage.append(contextString);
        }
        errorMessage += " but found ";
        errorMessage += token.getToken();
        errorMessage += " instead.";
        throw NexusParserException(errorMessage, token.getFilePosition(), token.getFileLine(), token.getFileColumn());
    }
}

// Advances the token, and raise an exception if it is not a semicolon sign.
void NexusParserBlock::demandEndSemicolon(NexusParserToken &token, QString contextString)
{
    token.getNextToken();
    if (!token.equals(";")) {
        errorMessage = "Expecting ';' to terminate the ";
        errorMessage.append(contextString);
        errorMessage += " command, but found ";
        errorMessage += token.getToken();
        errorMessage += " instead.";
        throw NexusParserException(errorMessage, token.getFilePosition(), token.getFileLine(), token.getFileColumn());
    }
}

// Advances the token, and raise an exception if it is not a semicolon sign.
int NexusParserBlock::demandPositiveInt(NexusParserToken &token, QString contextString)
{
    token.getNextToken();
    int i = token.getToken().toInt();
    if (i <= 0){
        errorMessage.append(contextString);
        errorMessage += " must be a number greater than 0. Found ";
        errorMessage += token.getToken();
        errorMessage += " instead.";
        throw NexusParserException(errorMessage, token.getFilePosition(), token.getFileLine(), token.getFileColumn());
    }
    return i;
}

// This base class version simply returns 0 but a derived class should override this function if it needs to construct
// and run a NexusParserSetReader object to read a set involving characters. The NexusParserSetReader object may need to use this
// function to look up a character label encountered in the set. A class that overrides this method should return the
// character index in the range [1..nchar].
int NexusParserBlock::charLabelToNumber(QString)
{
    return 0;
}

// This base class version simply returns 0, but a derived class should override this function if it needs to construct
// and run a NexusParserSetReader object to read a set involving taxa. The NexusParserSetReader object may need to use this function to
// look up a taxon label encountered in the set. A class that overrides this method should return the taxon index in
// the range [1..ntax].
int NexusParserBlock::taxonLabelToNumber(QString)
{
    return 0;
}
