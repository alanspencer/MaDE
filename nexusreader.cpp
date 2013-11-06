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
#include "nexusreadertaxablock.h"
#include "nexusreadercharactersblock.h"

NexusReader::NexusReader(QString fname, MainWindow *mw, Settings *s)
{
    mainwindow = mw;
    settings = s;
    filename = fname;
    blockList = NULL;
    currentBlock = NULL;

    nexusReaderLogMesssage(QString("starting NEXUS Reader on file \"%1\".").arg(filename));
}

// Add a block reader
void NexusReader::addBlock(QString blockID)
{
    if(!blocksToLoad.contains(blockID)) {
        blocksToLoad.append(blockID);
    }
}

void NexusReader::loadBlocks()
{
    NexusReaderBlock *block;
    NexusReaderTaxaBlock *taxaBlock;
    NexusReaderCharactersBlock *characterBlock;
    bool taxaBlockLoaded = false;
    bool assumptionsBlockLoaded = false;
    bool charactersBlockLoaded = false;

    for(int i = 0; i < blocksToLoad.count(); i++)
    {
        if(blocksToLoad[i] == "TAXA") {
            block = taxaBlock = new NexusReaderTaxaBlock();
            taxaBlockLoaded = true;
        } else if (blocksToLoad[i] == "CHARACTERS" && taxaBlockLoaded) {
            block = characterBlock = new NexusReaderCharactersBlock(taxaBlock);
            charactersBlockLoaded = true;
        }

        if (block != NULL) {
            block->setNexusReader(this);
            if (!blockList) {
                blockList = block;
            } else {
                // Add new block to end of list
                NexusReaderBlock *current;
                for (current = blockList; current && current->next;){
                    current = current->next;
                }
                current->next = block;
            }
        }
    }
}

QMap<QString, QVariant> NexusReader::getBlockData(QString blockID, int blockKey = 0)
{

    NexusReaderBlockList blockUsedList;
    if (blockIDToBlockList.contains(blockID)){
        blockUsedList = blockIDToBlockList.value(blockID);
        if (blockUsedList.count() == 1) {
            return blockUsedList[0]->getData();
        } else if (blockUsedList.count() > 1 && blockKey < blockUsedList.count()) {
            return blockUsedList[blockKey]->getData();
        }
    }

    QMap<QString, QVariant> data;
    return data;
}

int NexusReader::getBlockCount(QString blockID)
{
    return blockIDToBlockList.value(blockID).count();
}

// Returns a map from all block ids that have been read to all instances that the NexusReader knows have been read and
// have NOT been cleared.
NexusReaderBlockIDToBlockList NexusReader::getUsedBlocks()
{
    return blockIDToBlockList;
}



void NexusReader::addBlockToUsedBlockList(const QString &blockID, NexusReaderBlock *block)
{
    if (blockIDToBlockList.contains(blockID)) {
        NexusReaderBlockList exsitingBlockList;
        exsitingBlockList = blockIDToBlockList.value(blockID);
        exsitingBlockList.append(block);
        blockIDToBlockList.insert(blockID, exsitingBlockList);
    } else {
        NexusReaderBlockList newBlockList;
        newBlockList.append(block);
        blockIDToBlockList.insert(blockID, newBlockList);
    }
}

int NexusReader::removeBlockFromUsedBlockList(NexusReaderBlock *block)
{
    int totalDel = 0;
    int before;
    int after;
    QList<QString> keysToDel;

    NexusReaderBlockIDToBlockList::iterator i;
    for (i = blockIDToBlockList.begin(); i != blockIDToBlockList.end(); ++i) {
        NexusReaderBlockList & brl = i.value();
        before = brl.size();
        brl.removeAll(block);
        after = brl.size();
        if (after == 0){
            keysToDel.append(i.key());
        }
        totalDel += before - after;
    }
    for (int i = 0; i < keysToDel.count(); i++) {
        blockIDToBlockList.remove(keysToDel[i]);
    }

    return totalDel;
}

// Reads the NxsReader data file from the input stream provided by `token'. This function is responsible for reading
// through the name of a each block. Once it has read a block name, it searches for a block object to
// handle reading the remainder of the block's contents. The block object is responsible for reading the END or
// ENDBLOCK command as well as the trailing semicolon. This function also handles reading comments that are outside
// of blocks, as well as the initial "#NEXUS" keyword.
bool NexusReader::execute()
{
    currentBlock = NULL;
    QString errorMessage;

    loadBlocks();

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

        if (!token->equals("#NEXUS")) {
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

            if (token->equals("BEGIN")) {

                token->getNextToken();
                QString currentBlockName = token->getToken().toUpper();
                nexusReaderLogMesssage(QString("looking for BLOCK reader for block called \"%1\".").arg(currentBlockName));

                // Find Block Class to reader BLOCK
                for (currentBlock = blockList; currentBlock != NULL; currentBlock = currentBlock->next)
                {
                    if (token->equals(currentBlock->getID())){
                        break;
                    }
                }
                nexusReaderLogMesssage(QString("found a BLOCK called \"%1\" on line %2.").arg(currentBlockName).arg(token->getFileLine()));

                if (currentBlock == NULL) {
                    skippingBlock(currentBlockName);
                    if (!readUntilEndblock(token, currentBlockName)) {
                        return false;
                    }
                } else if (currentBlock->getEnabled()) {
                    if (!enteringBlock(currentBlockName)){
                        skippingBlock(currentBlockName);
                        if(!readUntilEndblock(token,currentBlockName)){
                            return false;
                        }
                    } else {
                        removeBlockFromUsedBlockList(currentBlock);
                        currentBlock->reset();

                        try {
                            currentBlock->read(token);
                            // Add to run block list?
                            addBlockToUsedBlockList(currentBlockName, currentBlock);
                        } catch (NexusReaderException x) {
                            if (currentBlock->errorMessage.length() > 0) {
                                nexusReaderLogError(currentBlock->errorMessage, x.filePos, x.fileLine, x.fileCol);
                            } else {
                                nexusReaderLogError(x.msg, x.filePos, x.fileLine, x.fileCol);
                            }
                            currentBlock->reset();
                            currentBlock = NULL;
                            return false;
                        }
                        exitingBlock(currentBlockName);
                        postBlockReadingHook(currentBlock);
                    }
                } else {
                    skippingDisabledBlock(currentBlockName);
                    if(!readUntilEndblock(token,currentBlockName)){
                        return false;
                    }
                }
                currentBlock = NULL;
            } else if (token->equals("&SHOWALL")) {
                // Used for debugging block...

            } else if (token->equals("&LEAVE")) {
                break;
            }
            qApp->processEvents();
        }


        int nMilliSeconds = runTimer.elapsed();
        nexusReaderLogMesssage(QString("NEXUS Reader has executed and returned with no fatal errors in %1 ms.").arg(nMilliSeconds));


        return true;
    }
}

// Called by the NexusReader object when a block named `blockName' is entered. Allows derived class overriding this
// function to notify user of progress in parsing the NEXUS file. Also gives program the opportunity to ask user if it
// is ok to purge data currently contained in this block. If user is asked whether existing data should be deleted, and
// the answer comes back no, then then the overrided function should return false, otherwise it should return true.
// This (base class) version always returns true.
bool NexusReader::enteringBlock(QString currentBlockName)
{
    nexusReaderLogMesssage(QString("entering BLOCK called \"%1\"...").arg(currentBlockName));
    return true;
}

// Called by the NexusReader object when a block named `blockName' is being exited. Allows derived class overriding this
// function to notify user of progress in parsing the NEXUS file.
void NexusReader::exitingBlock(QString currentBlockName)
{
    nexusReaderLogMesssage(QString("exiting BLOCK called \"%1\"...").arg(currentBlockName));
}

// Called after `block' has returned from NexusReader::read()
void NexusReader::postBlockReadingHook(NexusReaderBlock *block)
{
    nexusReaderLogMesssage(QString("running post-read hook on BLOCK called \"%1\"...").arg(block->getID()));

}

void NexusReader::skippingBlock(QString currentBlockName)
{
    nexusReaderLogMesssage(QString("no reader found for BLOCK called \"%1\", skipping BLOCK.").arg(currentBlockName));
}

void NexusReader::skippingDisabledBlock(QString currentBlockName)
{
    nexusReaderLogMesssage(QString("BLOCK called \"%1\" is disabled, skipping BLOCK.").arg(currentBlockName));
}

bool NexusReader::readUntilEndblock(NexusReaderToken *token, QString currentBlockName)
{
    for (;;)
    {
        token->getNextToken();
        if (token->equals("END") || token->equals("ENDBLOCK")){
            nexusReaderLogMesssage(QString("found \"END\" or \"ENDBLOCK\" for BLOCK called \"%1\" on line %2.").arg(currentBlockName).arg(token->getFileLine()));
            token->getNextToken();
            if (!token->equals(";")){
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
