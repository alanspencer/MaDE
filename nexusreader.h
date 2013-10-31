#ifndef NEXUSREADER_H
#define NEXUSREADER_H

#include <QWidget>
#include <QFile>

/*------------------------------------------------------------------------------------/
 * Single load point for all NEXUS parsing partly based on work by Paul O. Lewis as
 * part of NCL (Nexus Class Library) v2.0.04 which is available under a GNU General
 * Public License. http://sourceforge.net/projects/ncl/
 *
 * Set all variables needed for all other classes/headers, then include classes/headers
 *-----------------------------------------------------------------------------------*/

// Maximum number of states that can be stored; the only limitation is that this
// number be less than the maximum size of an int (not likely to be a problem).
// A good number for this is 76, which is 96 (the number of distinct symbols
// able to be input from a standard keyboard) less 20 (the number of symbols
// symbols disallowed by the NEXUS standard for use as state symbols)
#define NEXUS_MAX_STATES 76

#include "nexusreaderexception.h"
#include "nexusreadertoken.h"
#include "nexusreaderblock.h"

#include <mainwindow.h>
#include <settings.h>

class NexusReaderBlock;

class NexusReader
{
public:
    NexusReader(QString filename, MainWindow *mw, Settings *s);

    MainWindow *mainwindow;
    Settings *settings;
    QString filename;
    NexusReaderToken *token;

    bool execute();

    void nexusReaderLogError(QString message, qint64 filePos, qint64	fileLine, qint64 fileCol);
    void nexusReaderLogMesssage(QString message);

private:
    void addBlock(NexusReaderBlock *block);
    bool readUntilEndblock(NexusReaderToken *token, QString currentBlockName);

    QList<NexusReaderBlock*> blockList;

};

#endif // NEXUSREADER_H
