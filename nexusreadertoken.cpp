#include "nexusreadertoken.h"

/*----------------------------------------------------------------------------------------------------------------------
* NexusReaderToken objects are used by NexusReader to extract words (tokens) from a NEXUS data file. NexusToken objects
* know to correctly skip NEXUS comments and understand NEXUS punctuation, making reading a NEXUS file as simple as
* repeatedly calling the getNextToken() function and then interpreting the token returned.
*----------------------------------------------------------------------------------------------------------------------*/

NexusReaderToken::NexusReaderToken(QTextStream *in)
{
    input = in;

    atEndOfFile = false;
    atEndOfLine = false;
    fileCol = Q_INT64_C(1);
    fileLine = Q_INT64_C(1);
    filePos = Q_INT64_C(0);
    labileFlags	= 0;
    saved = '\0';
    special = '\0';

    whitespace[0]  = ' ';
    whitespace[1]  = '\t';
    whitespace[2]  = '\n';
    whitespace[3]  = '\0';

    punctuation[0]	= '(';
    punctuation[1]	= ')';
    punctuation[2]	= '[';
    punctuation[3]	= ']';
    punctuation[4]	= '{';
    punctuation[5]	= '}';
    punctuation[6]	= '/';
    punctuation[7]	= '\\';
    punctuation[8]	= ',';
    punctuation[9]	= ';';
    punctuation[10]	= ':';
    punctuation[11]	= '=';
    punctuation[12]	= '*';
    punctuation[13]	= '\'';
    punctuation[14]	= '"';
    punctuation[15]	= '`';
    punctuation[16]	= '+';
    punctuation[17]	= '-';
    punctuation[18]	= '<';
    punctuation[19]	= '>';
    punctuation[20]	= '\0';
}

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

// Returns token in upper case (default), set toUpper to false to keep case
QString NexusReaderToken::getToken(bool toUpper)
{
    if (!toUpper) {
        return token.toUpper();
    } else {
        return token;
    }
}


/*----------------------------------------------------------------------------------------------------------------------
*	Reads next character from file and does all of the following before returning it to the calling function:
*
*	o if character read is either a carriage return or line feed, the variable line is incremented by one and the
*	  variable fileCol is reset to zero
*	o if character read is a carriage return, and a peek at the next character to be read reveals that it is a line
*	  feed, then the next (line feed) character is read
*	o if either a carriage return or line feed is read, the character returned to the calling function is '\n' if
*	  character read is neither a carriage return nor a line feed, fileCol is incremented by one and the character is
*	  returned as is to the calling function
*	o in all cases, the variable filePos is updated using a call to the pos function of QFile.
*/

QChar NexusReaderToken::getNextChar()
{
    QChar ch, chNext;

    ch = input->read(Q_INT64_C(1)).at(0);
    filePos = input->pos();
    chNext = input->read(Q_INT64_C(1)).at(0);
    input->seek(filePos);

    if (ch == 13 || ch == 10) {
        fileLine++;
        fileCol = Q_INT64_C(1);

        if (ch == 13 && chNext == 10){
            ch = input->read(Q_INT64_C(1)).at(0);
            filePos = input->pos();
        }
        atEndOfLine = true;
    } else if (ch == EOF){
        atEndOfFile = true;
    } else {
        fileCol++;
        atEndOfLine = false;
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

    QChar ch = ' ';

    if (saved == '\0' || isWhitespace(saved)) {
        // Skip leading whitespace
        while(isWhitespace(ch) && !atEndOfFile)
            ch = getNextChar();
        saved = ch;
    }
}

void NexusReaderToken::resetToken()
{
    token.clear();
}

/*------------------------------------------------------------------------------------/
 * Character Matching Functions
 *-----------------------------------------------------------------------------------*/

bool NexusReaderToken::searchForCharInStr(QString searchIn, QChar searchFor)
{
    const QRegExp regexp("[" + QRegExp::escape(searchIn) + "]+");
    return regexp.exactMatch(QString(searchFor));
}

bool NexusReaderToken::isWhitespace(QChar ch)
{
    bool isWhitespace = false;

    // If ch is found in the whitespace array, it's whitespace
    if (searchForCharInStr(whitespace, ch))
        isWhitespace = true;

    // Unless of course ch is the newline character and we're currently treating newlines as darkspace!
    if (labileFlags & newlineIsToken && ch == '\n')
        isWhitespace = false;

    return isWhitespace;
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

        // Is it a graphic symbol?
        bool isGraph;
        if (ch.isPrint() || ch.isSymbol()) {
            isGraph = true;
        } else {
            isGraph = false;
        }

        if (!isGraph) {
            return true;
        } else if (searchForCharInStr(QString("\'[(){}\"-]/\\,;:=*`+<>"), ch)){
            // ' and [ always need quotes.  other punctuation needs quotes if it is in a word of length > 1
            if (ch == '\'' || ch == '['){
                return true;
            }
            return (str.length() > 1);
        }
    }
    return false;
}

