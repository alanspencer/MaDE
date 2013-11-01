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

#ifndef NEXUSREADERTOKEN_H
#define NEXUSREADERTOKEN_H

#include <QTextStream>

#include "nexusreader.h"

class NexusReaderToken
{
public:
    NexusReaderToken(QString d);

    static QString escapeString(const QString &);
    static QString getQuoted(const QString &);
    static bool needsQuotes(const QString &);

    qint64 getFileColumn() const;
    qint64 getFilePosition() const;
    qint64 getFileLine() const;
    bool getAtEndOfFile();
    bool getAtEndOfLine();
    QString getToken(bool respectCase = true);
    void getNextToken();
    void resetToken();
    bool tokenEquals(QString str, bool respectCase = true);
    void setLabileFlagBit(int bit);

    virtual void outputComment(const QString str);

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
    void appendToToken(QChar ch);
    void appendToComment(QChar ch);
    QChar getNextChar();
    void getQuoted();
    void getComment();
    void getParentheticalToken();
    void getCurlyBracketedToken();
    void getDoubleQuotedToken();
    bool isWhitespace(QChar ch);
    bool isPunctuation(QChar ch);

private:
    QString nexusData;

    bool searchForCharInList(QList<QChar> searchIn, QChar searchFor);

    qint64 filePos;         // current file position
    qint64 fileLine;        // current file line
    qint64 fileCol;         // current column in current line (refers to column immediately following token just read)

    QString token;          // the character buffer used to store the current token
    QString comment;        // temporary buffer used to store output comments while they are being built

    QChar saved;             // either '\0' or is last character read from input stream
    bool atEndOfFile;       // true if end of file has been encountered
    bool atEndOfLine;       // true if newline encountered while newlineIsToken labile flag set
    QChar special;           // ad hoc punctuation character; default value is '\0'
    int labileFlags;        // storage for flags in the NexusTokenFlags enum
    QList<QChar> punctuation;   // stores the 20 NEXUS punctuation characters
    QList<QChar> whitespace;     // stores the 3 whitespace characters: blank space, tab and newline
};

#endif // NEXUSREADERTOKEN_H
