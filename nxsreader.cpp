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

#include "ncl.h"

NxsReader::NxsReader(MainWindow *mw, Settings *s)
{
    mainwindow = mw;
    settings = s;
    blockList = NULL;
    currentBlock = NULL;

    NxsLogMesssage(QString("starting NEXUS Class Library."));
}


// Add a block reader
void NxsReader::addBlock(QString blockID)
{
    if(!blocksToLoad.contains(blockID)) {
        blocksToLoad.append(blockID);
    }
}

void NxsReader::loadBlocks()
{
    NxsBlock *block;
    NxsTaxaBlock *taxaBlock;
    NxsCharactersBlock *characterBlock;
    bool taxaBlockLoaded = false;
    bool assumptionsBlockLoaded = false;
    bool charactersBlockLoaded = false;

    for(int i = 0; i < blocksToLoad.count(); i++)
    {
        if(blocksToLoad[i] == "TAXA") {
            block = taxaBlock = new NxsTaxaBlock();
            taxaBlockLoaded = true;
        } else if (blocksToLoad[i] == "CHARACTERS" && taxaBlockLoaded) {
            block = characterBlock = new NxsCharactersBlock(taxaBlock);
            charactersBlockLoaded = true;
        }

        if (block != NULL) {
            block->setNxsReader(this);
            if (!blockList) {
                blockList = block;
            } else {
                // Add new block to end of list
                NxsBlock *current;
                for (current = blockList; current && current->next;){
                    current = current->next;
                }
                current->next = block;
            }
        }
    }
}

QMap<QString, QVariant> NxsReader::getBlockData(QString blockID, int blockKey = 0)
{

    NxsBlockList blockUsedList;
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

int NxsReader::getBlockCount(QString blockID)
{
    return blockIDToBlockList.value(blockID).count();
}

// Returns a map from all block ids that have been read to all instances that the Nxs knows have been read and
// have NOT been cleared.
NxsBlockIDToBlockList NxsReader::getUsedBlocks()
{
    return blockIDToBlockList;
}



void NxsReader::addBlockToUsedBlockList(const QString &blockID, NxsBlock *block)
{
    if (blockIDToBlockList.contains(blockID)) {
        NxsBlockList exsitingBlockList;
        exsitingBlockList = blockIDToBlockList.value(blockID);
        exsitingBlockList.append(block);
        blockIDToBlockList.insert(blockID, exsitingBlockList);
    } else {
        NxsBlockList newBlockList;
        newBlockList.append(block);
        blockIDToBlockList.insert(blockID, newBlockList);
    }
}

int NxsReader::removeBlockFromUsedBlockList(NxsBlock *block)
{
    int totalDel = 0;
    int before;
    int after;
    QList<QString> keysToDel;

    NxsBlockIDToBlockList::iterator i;
    for (i = blockIDToBlockList.begin(); i != blockIDToBlockList.end(); ++i) {
        NxsBlockList & brl = i.value();
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
bool NxsReader::execute(NxsToken &token)
{
    currentBlock = NULL;
    QString errorMessage;

    loadBlocks();

    // Timmer
    QTime runTimer;
    runTimer.start();

    try
    {
        token.getNextToken();
    }
    catch (NxsException x)
    {
        NxsLogError(token.errorMessage, 0, 0, 0);
        return false;
    }

    if (!token.equals("#NEXUS")) {
        errorMessage = "Expecting \"#NEXUS\" to be the first token in the file, but found \"";
        errorMessage += token.getToken();
        errorMessage += "\" instead";
        NxsLogError(errorMessage, token.getFilePosition(), token.getFileLine(), token.getFileColumn());
        return false;
    } else {
        NxsLogMesssage("found \"#NEXUS\" as the first token in the file.");
    }

    // Start searching for blocks
    NxsLogMesssage("searching for BLOCKS...");
    for (;;)
    {
        token.setLabileFlagBit(NxsToken::saveCommandComments);
        token.getNextToken();

        if (token.getAtEndOfFile()){
            break;
        }

        if (token.equals("BEGIN")) {

            token.getNextToken();
            QString currentBlockName = token.getToken().toUpper();
            NxsLogMesssage(QString("looking for BLOCK reader for block called \"%1\".").arg(currentBlockName));

            // Find Block Class to reader BLOCK
            for (currentBlock = blockList; currentBlock != NULL; currentBlock = currentBlock->next)
            {
                if (token.equals(currentBlock->getID())){
                    break;
                }
            }
            NxsLogMesssage(QString("found a BLOCK called \"%1\" on line %2.").arg(currentBlockName).arg(token.getFileLine()));

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
                    } catch (NxsException x) {
                        if (currentBlock->errorMessage.length() > 0) {
                            NxsLogError(currentBlock->errorMessage, x.filePos, x.fileLine, x.fileCol);
                        } else {
                            NxsLogError(x.msg, x.filePos, x.fileLine, x.fileCol);
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
        } else if (token.equals("&SHOWALL")) {
            // Used for debugging block...

        } else if (token.equals("&LEAVE")) {
            break;
        }
        qApp->processEvents();
    }


    int nMilliSeconds = runTimer.elapsed();
    NxsLogMesssage(QString("NEXUS Class Library has executed and returned with no fatal errors in %1 ms.").arg(nMilliSeconds));


    return true;
}

// Called by the Nxs object when a block named `blockName' is entered. Allows derived class overriding this
// function to notify user of progress in parsing the NEXUS file. Also gives program the opportunity to ask user if it
// is ok to purge data currently contained in this block. If user is asked whether existing data should be deleted, and
// the answer comes back no, then then the overrided function should return false, otherwise it should return true.
// This (base class) version always returns true.
bool NxsReader::enteringBlock(QString currentBlockName)
{
    NxsLogMesssage(QString("entering BLOCK called \"%1\"...").arg(currentBlockName));
    return true;
}

// Called by the Nxs object when a block named `blockName' is being exited. Allows derived class overriding this
// function to notify user of progress in parsing the NEXUS file.
void NxsReader::exitingBlock(QString currentBlockName)
{
    NxsLogMesssage(QString("exiting BLOCK called \"%1\"...").arg(currentBlockName));
}

// Called after `block' has returned from NxsReader::read()
void NxsReader::postBlockReadingHook(NxsBlock *block)
{
    NxsLogMesssage(QString("running post-read hook on BLOCK called \"%1\"...").arg(block->getID()));

}

void NxsReader::skippingBlock(QString currentBlockName)
{
    NxsLogMesssage(QString("no reader found for BLOCK called \"%1\", skipping BLOCK.").arg(currentBlockName));
}

void NxsReader::skippingDisabledBlock(QString currentBlockName)
{
    NxsLogMesssage(QString("BLOCK called \"%1\" is disabled, skipping BLOCK.").arg(currentBlockName));
}

bool NxsReader::readUntilEndblock(NxsToken token, QString currentBlockName)
{
    for (;;)
    {
        token.getNextToken();
        if (token.equals("END") || token.equals("ENDBLOCK")){
            NxsLogMesssage(QString("found \"END\" or \"ENDBLOCK\" for BLOCK called \"%1\" on line %2.").arg(currentBlockName).arg(token.getFileLine()));
            token.getNextToken();
            if (!token.equals(";")){
                QString errorMessage = QString("Expecting ';' after END or ENDBLOCK command, but found \"%1\" instead").arg(token.getToken());
                NxsLogError(errorMessage, token.getFilePosition(), token.getFileLine(), token.getFileColumn());
                return false;
            }
            return true;
        }
        if (token.getAtEndOfFile()){
            QString errorMessage = QString("Encountered end of file before END or ENDBLOCK in BLOCK called \"%1\".").arg(token.getToken());
            NxsLogError(errorMessage, token.getFilePosition(), token.getFileLine(), token.getFileColumn());
            return false;
        }
    }
}


// Called when an error is encountered in a NEXUS file. Allows program to give user details of the error as well as
// the precise location of the error via the application log.
void NxsReader::NxsLogError(QString message, qint64 filePos, qint64 fileLine, qint64 fileCol)
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
void NxsReader::NxsLogMesssage(QString message)
{
    // Write to main window application log
    mainwindow->logAppend("NEXUS Reader",QString("%1").arg(message));
}
