#ifndef NEXUSREADERTOKEN_H
#define NEXUSREADERTOKEN_H

#include <QTextStream>

#include "nexusreader.h"

class NexusReaderToken
{
public:
    NexusReaderToken(QTextStream *in);

    QString escapeString(const QString &);
    QString getQuoted(const QString &);
    bool needsQuotes(const QString &);

    qint64 getFileColumn() const;
    qint64 getFilePosition() const;
    qint64 getFileLine() const;

    QString getToken(bool toUpper = true);
    void getNextToken();
    QChar getNextChar();

    /* For use with the variable labileFlags */
    enum NexusReaderTokenFlags	{
        saveCommandComments = 0x0001,	/* if set, command comments of the form [&X] are not ignored but are instead saved as regular tokens (without the square brackets, however) */
        parentheticalToken = 0x0002,	/* if set, and if next character encountered is a left parenthesis, token will include everything up to the matching right parenthesis */
        curlyBracketedToken = 0x0004,	/* if set, and if next character encountered is a left curly bracket, token will include everything up to the matching right curly bracket */
        doubleQuotedToken = 0x0008,     /* if set, grabs entire phrase surrounded by double quotes */
        singleCharacterToken = 0x0010,	/* if set, next non-whitespace character returned as token */
        newlineIsToken = 0x0020,        /* if set, newline character treated as a token and atEOL set if newline encountered */
        tildeIsPunctuation = 0x0040,	/* if set, tilde character treated as punctuation and returned as a separate token */
        useSpecialPunctuation = 0x0080,	/* if set, character specified by the data member special is treated as punctuation and returned as a separate token */
        hyphenNotPunctuation = 0x0100,	/* if set, the hyphen character is not treated as punctutation (it is normally returned as a separate token) */
        preserveUnderscores = 0x0200,	/* if set, underscore characters inside tokens are not converted to blank spaces (normally, all underscores are automatically converted to blanks) */
        ignorePunctuation = 0x0400      /* if set, the normal punctuation symbols are treated the same as any other darkspace characters */
    };

    QString errorMessage;

protected:
    void getQuoted();

private:

    void resetToken();
    bool searchForCharInStr(QString searchIn, QChar searchFor);
    bool isWhitespace(QChar ch);

    QTextStream *input;     // reference to input from which tokens will be read

    qint64 filePos;         // current file position
    qint64 fileLine;        // current file line
    qint64 fileCol;         // current column in current line (refers to column immediately following token just read)

    QString token;          // the character buffer used to store the current token
    QString comment;        // temporary buffer used to store output comments while they are being built

    QChar saved;             // either '\0' or is last character read from input stream
    bool atEndOfFile;       // true if end of file has been encountered
    bool atEndOfLine;       // true if newline encountered while newlineIsToken labile flag set
    QChar special;           // ad hoc punctuation character; default value is '\0'
    int labileFlags;        // storage for flags in the NxsTokenFlags enum
    QString punctuation;   // stores the 20 NEXUS punctuation characters
    QString whitespace;     // stores the 3 whitespace characters: blank space, tab and newline
};

#endif // NEXUSREADERTOKEN_H
