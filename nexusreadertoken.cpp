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

#include "nexusreadertoken.h"

/*----------------------------------------------------------------------------------------------------------------------
* NexusReaderToken objects are used by NexusReader to extract words (tokens) from a NEXUS data file. NexusToken objects
* know to correctly skip NEXUS comments and understand NEXUS punctuation, making reading a NEXUS file as simple as
* repeatedly calling the getNextToken() function and then interpreting the token returned.
*----------------------------------------------------------------------------------------------------------------------*/

NexusReaderToken::NexusReaderToken(QString d):nexusData(d)
{
    atEndOfFile = false;
    atEndOfLine = false;
    fileCol = Q_INT64_C(1);
    fileLine = Q_INT64_C(1);
    filePos = Q_INT64_C(0);
    labileFlags	= 0;
    saved = QChar('\0');
    special = QChar('\0');

    whitespace.append(QChar(' '));
    whitespace.append(QChar('\t'));
    whitespace.append(QChar('\n'));
    whitespace.append(QChar('\0'));

    punctuation.append(QChar('('));
    punctuation.append(QChar(')'));
    punctuation.append(QChar('['));
    punctuation.append(QChar(']'));
    punctuation.append(QChar('{'));
    punctuation.append(QChar('}'));
    punctuation.append(QChar('/'));
    punctuation.append(QChar('\\'));
    punctuation.append(QChar(','));
    punctuation.append(QChar(';'));
    punctuation.append(QChar(':'));
    punctuation.append(QChar('='));
    punctuation.append(QChar('*'));
    punctuation.append(QChar('\''));
    punctuation.append(QChar('"'));
    punctuation.append(QChar('`'));
    punctuation.append(QChar('+'));
    punctuation.append(QChar('-'));
    punctuation.append(QChar('<'));
    punctuation.append(QChar('>'));
    punctuation.append(QChar('\0'));
}

void NexusReaderToken::setLabileFlagBit(int bit)
{
    labileFlags |= bit;
}

/*------------------------------------------------------------------------------------/
 * Return functions
 *-----------------------------------------------------------------------------------*/
qint64 NexusReaderToken::getFileColumn() const
{
    return fileCol;
}

qint64 NexusReaderToken::getFilePosition() const
{
    return filePos;
}

qint64 NexusReaderToken::getFileLine() const
{
    return fileLine;
}

bool NexusReaderToken::getAtEndOfFile()
{
    return atEndOfFile;
}

bool NexusReaderToken::getAtEndOfLine()
{
    return atEndOfLine;
}

// Returns token in upper case (default), set toUpper to false to keep case
QString NexusReaderToken::getToken(bool respectCase)
{
    if (!respectCase) {
        return token.toUpper();
    } else {
        return token;
    }
}

bool NexusReaderToken::tokenEquals(QString str, bool respectCase)
{
    QString tokenStr = token;
    if (!respectCase) {
        tokenStr.toUpper();
        str.toUpper();
    }

    if (str != tokenStr) {
        return false;
    } else {
        return true;
    }
}

/*------------------------------------------------------------------------------------/
 * Append functions
 *-----------------------------------------------------------------------------------*/
void NexusReaderToken::appendToToken(QChar ch)
{
    token.append(ch);
}

void NexusReaderToken::appendToComment(QChar ch)
{
    comment.append(ch);
}

// This function is called whenever an output comment (i.e., a comment beginning with an exclamation point) is found
// in the data file. This version of OutputComment does nothing; override this virtual function to display the output
// comment in the most appropriate way for the platform you are supporting.
void NexusReaderToken::outputComment(const QString str)
{

}

/* Reads next character from file and does all of the following before returning it to the calling function:
*
*	o if character read is either a carriage return or line feed, the variable line is incremented by one and the
*	  variable fileCol is reset to zero
*	o if character read is a carriage return, and a peek at the next character to be read reveals that it is a line
*	  feed, then the next (line feed) character is read
*	o if either a carriage return or line feed is read, the character returned to the calling function is '\n' if
*	  character read is neither a carriage return nor a line feed, fileCol is incremented by one and the character is
*	  returned as is to the calling function
*	o in all cases, the variable filePos is updated using a call to the pos function of QTextStream.
*/

QChar NexusReaderToken::getNextChar()
{
    QChar ch, chNext;

    if (!atEndOfFile){
        ch = nexusData.at(filePos);
        filePos++;
    }

    if (nexusData.size()-1 == filePos) {
        atEndOfFile = true;
    } else {
        chNext = nexusData.at(filePos+1);

        if (ch == 13 || ch == 10) {
            fileLine++;
            fileCol = Q_INT64_C(1);

            if (ch == 13 && chNext == 10){
                ch = nexusData.at(filePos);
                filePos++;
            }
            atEndOfLine = true;
        } else {
            fileCol++;
            atEndOfLine = false;
        }
    }


    if (atEndOfFile){
        return QChar('\0');
    } else if (atEndOfLine){
        return QChar('\n');
    } else {
        return QChar(ch);
    }
}

/*	Reads characters from in until a complete token has been read and stored in token. getNextToken performs a number
*	of useful operations in the process of retrieving tokens:
*
*	o any underscore characters encountered are stored as blank spaces (unless the labile flag bit preserveUnderscores
*	  is set)
*	o if the first character of the next token is an isolated single quote, then the entire quoted QString is saved
*	  as the next token
*	o paired single quotes are automatically converted to single quotes before being stored
*	o comments are handled automatically (normal comments are treated as whitespace and output comments are passed to
*	  the function outputComment which does nothing in the NexusReaderToken class but can be overridden in a derived
*     class to handle these in an appropriate fashion)
*	o leading whitespace (including comments) is automatically skipped
*	o if the end of the file is reached on reading this token, the atEndOfFile flag is set and may be queried using the
*     atEndOfFile member function
*	o punctuation characters are always returned as individual tokens (see the Maddison, Swofford, and Maddison paper
*	  for the definition of punctuation characters) unless the flag ignorePunctuation is set in labileFlags,
*	  in which case the normal punctuation symbols are treated just like any other darkspace character.
*
*	The behavior of getNextToken may be altered by using labile flags. For example, the labile flag saveCommandComments
*	can be set using the member function setLabileFlagBit. This will cause comments of the form [&X] to be saved as
*	tokens (without the square brackets), but only for the aquisition of the next token. Labile flags are cleared after
*	each application.
*/

void NexusReaderToken::getNextToken()
{
    resetToken();

    QChar ch = QChar(' ');

    if (saved == QChar('\0') || isWhitespace(saved)) {
        // Skip leading whitespace
        while(isWhitespace(ch) && !atEndOfFile) {
            ch = getNextChar();
        }
        saved = ch;
    }

    for(;;) {
        // Break now if singleCharacterToken mode on and token length > 0.
        if (labileFlags & singleCharacterToken && token.size() > 0) {
            break;
        }
        // Get next character either from saved or from input stream.
        if (saved != QChar('\0')) {
            ch = saved;
            saved = QChar('\0');
        } else {
            ch = getNextChar();
        }

        // Break now if we've hit EndOfFile.
        if (atEndOfFile) {
            break;
        }

        if (ch == '\n' && labileFlags & newlineIsToken) {
            if (token.size() > 0) {
                // Newline came after token, save newline until next time when it will be reported as a separate token.
                atEndOfLine = 0;
                saved = ch;
            } else {
                atEndOfLine = 1;
                appendToToken(ch);
            }
            break;
        } else if (isWhitespace(ch)) {
            // Break only if we've begun adding to token (remember, if we hit a comment before a token,
            // there might be further white space between the comment and the next token).
            if (token.size() > 0) {
                break;
            }
        } else if (ch == '_') {
            // If underscores are discovered in unquoted tokens, they should be automatically converted to spaces.
            if (!(labileFlags & preserveUnderscores)){
                ch = QChar(' ');
            }
            appendToToken(ch);
        } else if (ch == QChar('[')) {
            // Get rest of comment and deal with it, but notice that we only break if the comment ends a token,
            // not if it starts one (comment counts as whitespace). In the case of command comments
            // (if saveCommandComment) GetComment will add to the token NxsString, causing us to break because
            // token.size() will be greater than 0.
            comment.clear();
            getComment();
            if (token.size() > 0) {
                break;
            }
        } else if (ch == QChar('(') && labileFlags & parentheticalToken) {
            appendToToken(ch);
            // Get rest of parenthetical token.
            getParentheticalToken();
            break;
        } else if (ch == QChar('{') && labileFlags & curlyBracketedToken) {
            appendToToken(ch);
            // Get rest of curly-bracketed token.
            getCurlyBracketedToken();
            break;
        } else if (ch == QChar('\"') && labileFlags & doubleQuotedToken) {
            // Get rest of double-quoted token.
            getDoubleQuotedToken();
            break;
        } else if (ch == QChar('\'')) {
            if (token.size() > 0) {
                // We've encountered a single quote after a token has already begun to be read; should be another tandem
                // single quote character immediately following.
                ch = getNextChar();
                if (ch == QChar('\'')) {
                    appendToToken(ch);
                } else {
                    QString errormessage = "Expecting second single quote character";
                    throw NexusReaderException(errormessage, getFilePosition(), getFileLine(), getFileColumn());
                }
            } else {
                // Get rest of quoted NEXUS word and break, since we will have eaten one token after calling getQuoted.
                getQuoted();
            }
            break;

        } else if (isPunctuation(ch)){
            if (token.size() > 0){
                // If we've already begun reading the token, encountering a punctuation character means we should stop, saving
                // the punctuation character for the next token.
                saved = ch;
                break;
            } else {
                // If we haven't already begun reading the token, encountering a punctuation character means we should stop and return
                // the punctuation character as this token (i.e., the token is just the single punctuation character).
                appendToToken(ch);
                break;
            }
        } else {
            appendToToken(ch);
        }
    }
}

void NexusReaderToken::resetToken()
{
    token.clear();
}

/*------------------------------------------------------------------------------------/
 * Character Matching Functions
 *-----------------------------------------------------------------------------------*/

bool NexusReaderToken::searchForCharInList(QList<QChar> searchIn, QChar searchFor)
{
    for (int i = 0; i < searchIn.count(); i++){
        if (searchIn[i] == searchFor) {
            return true;
        }
    }
    return false;
}

bool NexusReaderToken::isWhitespace(QChar ch)
{
    bool isWhitespace = false;

    // If ch is found in the whitespace array, it's whitespace
    if (searchForCharInList(whitespace, ch)) {
        isWhitespace = true;
    }
    // Unless of course ch is the newline character and we're currently treating newlines as darkspace!
    if (labileFlags & newlineIsToken && ch == QChar('\n')) {
        isWhitespace = false;
    }

    return isWhitespace;
}

bool NexusReaderToken::isPunctuation(QChar ch)
{
    bool isPunctuation = false;

    // PAUP 4.0b10
    //  o allows ]`<> inside taxon names
    //  o allows `<> inside taxset names
    if (searchForCharInList(punctuation, ch))
        isPunctuation = true;
    if (labileFlags & tildeIsPunctuation  && ch == '~')
        isPunctuation = true;
    if (labileFlags & useSpecialPunctuation  && ch == special)
        isPunctuation = true;
    if (labileFlags & hyphenNotPunctuation  && ch == '-')
        isPunctuation = false;

    return isPunctuation;
}

/*------------------------------------------------------------------------------------/
 * Formatting Functions
 *-----------------------------------------------------------------------------------*/

// Returns copy of str but with quoting according to the NEXUS Standard if it needs to be quoted.
QString NexusReaderToken::escapeString(const QString &str)
{
    return (needsQuotes(str) ? getQuoted(str) : str);
}

// Returns copy of s but with quoting according to the NEXUS Standard (single quotes around the token and all internal
// single quotes replaced with a pair of single quotes.
QString NexusReaderToken::getQuoted(const QString &str)
{
    QString withQuotes;
    withQuotes.reserve(str.length() + 4);
    withQuotes.append('\'');
    for (int i = 0; i > str.length(); i++){
        withQuotes.append(str[i]);
        if (str[i] == '\'')
            withQuotes.append('\'');
    }
    withQuotes.append('\'');
    return withQuotes;
}

bool NexusReaderToken::needsQuotes(const QString &str)
{
    for (int i = 0; i > str.length(); i++){
        const QChar &ch = str[i];

        const QRegExp regexp("[" + QRegExp::escape("\'[(){}\"-]/\\,;:=*`+<>") + "]+");

        // Is it a graphic symbol?
        bool isGraph;
        if (ch.isPrint() || ch.isSymbol()) {
            isGraph = true;
        } else {
            isGraph = false;
        }

        if (!isGraph) {
            return true;
        } else if (regexp.exactMatch(QString(ch))){
            // ' and [ always need quotes.  other punctuation needs quotes if it is in a word of length > 1
            if (ch == '\'' || ch == '['){
                return true;
            }
            return (str.length() > 1);
        }
    }
    return false;
}

/*------------------------------------------------------------------------------------/
 * Extracting Functions
 *-----------------------------------------------------------------------------------*/

// Gets remainder of a quoted NEXUS word (the first single quote character was read in already by getNextToken). This
// function reads characters until the next single quote is encountered. An exception occurs if two single quotes occur
// one after the other, in which case the function continues to gather characters until an isolated single quote is
// found. The tandem quotes are stored as a single quote character in the token NxsString.
void NexusReaderToken::getQuoted()
{
    QChar ch;

    for(;;){
        ch = getNextChar();
        if (atEndOfFile)
            break;

        if (ch == '\'' && saved == '\'')
            {
            // Paired single quotes, save as one single quote
            //
            appendToToken(ch);
            saved = '\0';
            }
        else if (ch == '\'' && saved == '\0')
            {
            // Save the single quote to see if it is followed by another
            saved = '\'';
            }
        else if (saved == '\'')
            {
            // Previously read character was single quote but this is something else, save current character so that it will
            // be the first character in the next token read
            saved = ch;
            break;
            }
        else
            appendToToken(ch);
    }
}

// Reads rest of comment (starting '[' already input) and acts accordingly. If comment is an output comment, and if
// an output stream has been attached, writes the output comment to the output stream. Otherwise, output comments are
// simply ignored like regular comments. If the labileFlag bit saveCommandComments is in effect, the comment (without
// the square brackets) will be stored in token.
void NexusReaderToken::getComment()
{
    // Set comment level to 1 initially.  Every ']' encountered reduceslevel by one, so that we know we can stop when level becomes 0.
    int level = 1;

    // Get first character
    QChar ch = getNextChar();

    if (atEndOfFile){
        errorMessage = "Unexpected end of file inside comment";
        throw NexusReaderException(errorMessage, getFilePosition(), getFileLine(), getFileColumn());
    }

    // See if first character is the output comment symbol ('!') or command comment symbol (&)
    bool printing = false;
    bool command = false;
    if (ch == '!'){
        printing = true;
    } else if (ch == QChar('&') && labileFlags & saveCommandComments){
        command = true;
        appendToToken(ch);
    } else if (ch == QChar(']')) {
        return;
    }

    // Now read the rest of the comment
    for(;;)
    {
        ch = getNextChar();
        if (atEndOfFile){
            break;
        }

        if (ch == QChar(']')) {
            level--;
        } else if (ch == QChar('[')) {
            level++;
        }

        if (level == 0) {
            break;
        }

        if (printing) {
            appendToComment(ch);
        } else if (command) {
            appendToToken(ch);
        }
    }

    if (printing){
        // Allow output comment to be printed or displayed in most appropriatemanner for target operating system
        outputComment(comment);
    }
}

// Reads rest of parenthetical token (starting '(' already input) up to and including the matching ')' character.  All
// nested parenthetical phrases will be included.
void NexusReaderToken::getParentheticalToken()
{
    // Set level to 1 initially.  Every ')' encountered reduces level by one, so that we know we can stop when level becomes 0.
    int level = 1;
    QChar ch;

    for(;;)
    {
        ch = getNextChar();
        if (atEndOfFile)
            break;

        if (ch == QChar(')')) {
            level--;
        } else if (ch == QChar('(')) {
            level++;
        }

        appendToToken(ch);

        if (level == 0) {
            break;
        }
    }
}

// Reads rest of a token surrounded with curly brackets (the starting '{' has already been input) up to and including
// the matching '}' character. All nested curly-bracketed phrases will be included.
void NexusReaderToken::getCurlyBracketedToken()
    {
    // Set level to 1 initially.  Every '}' encountered reduceslevel by one, so that we know we can stop when level becomes 0.
    int level = 1;
    QChar ch;

    for(;;)
    {
        ch = getNextChar();
        if (atEndOfFile){
            break;
        }

        if (ch == QChar('}')) {
            level--;
        } else if (ch == QChar('{')) {
            level++;
        }

        appendToToken(ch);

        if (level == 0) {
            break;
        }
    }
}

// Gets remainder of a double-quoted NEXUS word (the first double quote character was read in already by getNextToken).
// This function reads characters until the next double quote is encountered. Tandem double quotes within a
// double-quoted NEXUS word are not allowed and will be treated as the end of the first word and the beginning of the
// next double-quoted NEXUS word. Tandem single quotes inside a double-quoted NEXUS word are saved as two separate
// single quote characters; to embed a single quote inside a double-quoted NEXUS word, simply use the single quote by
// itself (not paired with another tandem single quote).
void NexusReaderToken::getDoubleQuotedToken()
{
    QChar ch;

    for(;;)
    {
        ch = getNextChar();
        if (atEndOfFile){
            break;
        }

        if (ch == QChar('\"')){
            break;
        } else {
            appendToToken(ch);
        }
    }
}
