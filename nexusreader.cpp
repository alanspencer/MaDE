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

#include "nexusreader.h"

NexusReader::NexusReader(QString fname, MainWindow *mw, Settings *s)
{
    mainwindow = mw;
    settings = s;
    filename = fname;

    nexusReaderLogMesssage(QString("starting NEXUS Reader on file \"%1\".").arg(filename));
}

bool NexusReader::execute()
{
    QString errorMessage;

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        nexusReaderLogError("Unable to open .nex file in readonly text mode.", 0, 0, 0);
        return false;
    } else {
        nexusReaderLogMesssage(".nex file found and opened in readonly text mode.");

        // Timmer
        QTime runTimer;
        runTimer.start();

        // Create new NexusReaderToken form file data
        QTextStream in(&file);
        token = new NexusReaderToken(in.readAll());
        file.close();

        try
        {
            token->getNextToken();
        }
        catch (NexusReaderException x)
        {
            nexusReaderLogError(token->errorMessage, 0, 0, 0);
            return false;
        }

        if (!token->tokenEquals("#NEXUS")) {
            errorMessage = "Expecting \"#NEXUS\" to be the first token in the file, but found \"";
            errorMessage += token->getToken();
            errorMessage += "\" instead";
            nexusReaderLogError(errorMessage, token->getFilePosition(), token->getFileLine(), token->getFileColumn());
            return false;
        } else {
            nexusReaderLogMesssage("found \"#NEXUS\" as the first token in the file.");
        }

        // Start searching for blocks
        nexusReaderLogMesssage("searching for BLOCKS...");
        for (;;)
        {
            token->setLabileFlagBit(NexusReaderToken::saveCommandComments);
            token->getNextToken();

            if (token->getAtEndOfFile()){
                break;
            }

            if (token->tokenEquals("BEGIN")) {
                token->getNextToken();
                QString currentBlockName = token->getToken();
                nexusReaderLogMesssage(QString("found a BLOCK called \"%1\" on line %2.").arg(currentBlockName).arg(token->getFileLine()));

                // Lookup current block from blocklist
                bool blockReaderFound = false;
                if (!blockList.empty()) {
                    for (int i = 0; i < blockList.count(); i++)
                    {
                        if (token->tokenEquals(blockList[i]->getID())) {
                            blockReaderFound = true;
                            break;
                        }
                    }
                }
                if (blockReaderFound) {
                    nexusReaderLogMesssage(QString("loading BLOCK reader for block called \"%1\".").arg(currentBlockName));
                } else {
                    nexusReaderLogMesssage(QString("no reader found for BLOCK called \"%1\", skipping BLOCK.").arg(currentBlockName));
                    if (!readUntilEndblock(token, currentBlockName)) {
                        return false;
                    }
                }

            }
            qApp->processEvents();
        }


        int nMilliSeconds = runTimer.elapsed();
        nexusReaderLogMesssage(QString("NEXUS Reader has executed and returned with no fatal errors in %1 ms.").arg(nMilliSeconds));
        return true;
    }
}

bool NexusReader::readUntilEndblock(NexusReaderToken *token, QString currentBlockName)
{
    for (;;)
    {
        token->getNextToken();
        if (token->tokenEquals("END") || token->tokenEquals("ENDBLOCK")){
            nexusReaderLogMesssage(QString("found \"END\" or \"ENDBLOCK\" for BLOCK called \"%1\" on line %2.").arg(currentBlockName).arg(token->getFileLine()));
            token->getNextToken();
            if (!token->tokenEquals(";")){
                QString errorMessage = QString("Expecting ';' after END or ENDBLOCK command, but found \"%1\" instead").arg(token->getToken());
                nexusReaderLogError(errorMessage, token->getFilePosition(), token->getFileLine(), token->getFileColumn());
                return false;
            }
            return true;
        }
        if (token->getAtEndOfFile()){
            QString errorMessage = QString("Encountered end of file before END or ENDBLOCK in BLOCK called \"%1\".").arg(token->getToken());
            nexusReaderLogError(errorMessage, token->getFilePosition(), token->getFileLine(), token->getFileColumn());
            return false;
        }
    }
}

void NexusReader::addBlock(NexusReaderBlock *block)
{
    blockList.append(block);
}

// Called when an error is encountered in a NEXUS file. Allows program to give user details of the error as well as
// the precise location of the error via the application log.
void NexusReader::nexusReaderLogError(QString message, qint64 filePos, qint64 fileLine, qint64 fileCol)
{
    // Write to main window application log
    mainwindow->logAppend("NEXUS Reader",
                          QString("ERROR \"%1\" @ File Position = %2, File Line = %3, File Column = %4.")
                          .arg(message)
                          .arg(filePos)
                          .arg(fileLine)
                          .arg(fileCol)
                          );
}

// Called when a message is to be logged. Allows program to give user details of the message via the application log.
void NexusReader::nexusReaderLogMesssage(QString message)
{
    // Write to main window application log
    mainwindow->logAppend("NEXUS Reader",QString("%1").arg(message));
}
