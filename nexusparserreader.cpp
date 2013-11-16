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

NexusParserReader::NexusParserReader(MainWindow *mw, Settings *s)
{
    mainwindow = mw;
    settings = s;
    blockList = NULL;
    currentBlock = NULL;

    // State Sets
    defaultStandardStateSet = settings->getSetting("defaultStandardStateSet").toStringList();           // STANDARD
    allowedStandardStateSet = settings->getSetting("allowedStandardStateSet").toStringList();           // Allowed STANDARD
    defaultDNAStateSet = settings->getSetting("defaultDNAStateSet").toStringList();                     // DNA
    defaultRNAStateSet = settings->getSetting("defaultRNAStateSet").toStringList();                     // RNA
    defaultNucleotideStateSet = settings->getSetting("defaultNucleotideStateSet").toStringList();       // NUCLEOTIDE
    defaultProteinStateSet = settings->getSetting("defaultProteinStateSet").toStringList();             // PROTEIN

    // Equate Lists
    defaultDNAEquateStates = settings->getSetting("defaultDNAEquateStates").toStringList();                 // DNA
    defaultRNAEquateStates = settings->getSetting("defaultRNAEquateStates").toStringList();                 // RNA
    defaultNucleotideEquateStates = settings->getSetting("defaultNucleotideEquateStates").toStringList();   // NUCLEOTIDE
    defaultProteinEquateStates = settings->getSetting("defaultProteinEquateStates").toStringList();         // PROTEIN

    // Missing/Gap/Match Characters    
    defaultMissingCharacter = settings->getSetting("defaultMissingCharacter").toString().at(0);
    defaultGapCharacter = settings->getSetting("defaultGapCharacter").toString().at(0);
    defaultMatchCharacter = settings->getSetting("defaultMatchCharacter").toString().at(0);

    logMesssage(QString("starting NEXUS Class Library."));
}


// Add a block reader
void NexusParserReader::addBlock(QString blockID)
{
    if(!blocksToLoad.contains(blockID)) {
        blocksToLoad.append(blockID);
    }
}

void NexusParserReader::loadBlocks()
{
    NexusParserBlock *block;
    NexusParserTaxaBlock *taxaBlock;
    NexusParserCharactersBlock *characterBlock;
    bool taxaBlockLoaded = false;
    bool assumptionsBlockLoaded = false;
    bool charactersBlockLoaded = false;

    for(int i = 0; i < blocksToLoad.count(); i++)
    {
        if(blocksToLoad[i] == "TAXA") {
            block = taxaBlock = new NexusParserTaxaBlock(this);
            taxaBlockLoaded = true;
        } else if (blocksToLoad[i] == "CHARACTERS" && taxaBlockLoaded) {
            block = characterBlock = new NexusParserCharactersBlock(this, taxaBlock);
            charactersBlockLoaded = true;
        }

        if (block != NULL) {
            if (!blockList) {
                blockList = block;
            } else {
                // Add new block to end of list
                NexusParserBlock *current;
                for (current = blockList; current && current->next;){
                    current = current->next;
                }
                current->next = block;
            }
        }
    }
}

int NexusParserReader::getBlockCount(QString blockID)
{
    return blockIDToBlockList.value(blockID).count();
}

// Returns a map from all block ids that have been read to all instances that the Nxs knows have been read and
// have NOT been cleared.
NexusParserBlockIDToBlockList NexusParserReader::getUsedBlocks()
{
    return blockIDToBlockList;
}



void NexusParserReader::addBlockToUsedBlockList(const QString &blockID, NexusParserBlock *block)
{
    if (blockIDToBlockList.contains(blockID)) {
        NexusParserBlockList exsitingBlockList;
        exsitingBlockList = blockIDToBlockList.value(blockID);
        exsitingBlockList.append(block);
        blockIDToBlockList.insert(blockID, exsitingBlockList);
    } else {
        NexusParserBlockList newBlockList;
        newBlockList.append(block);
        blockIDToBlockList.insert(blockID, newBlockList);
    }
}

int NexusParserReader::removeBlockFromUsedBlockList(NexusParserBlock *block)
{
    int totalDel = 0;
    int before;
    int after;
    QList<QString> keysToDel;

    NexusParserBlockIDToBlockList::iterator i;
    for (i = blockIDToBlockList.begin(); i != blockIDToBlockList.end(); ++i) {
        NexusParserBlockList & brl = i.value();
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

// Reads the NexusParserReader data file from the input stream provided by `token'. This function is responsible for reading
// through the name of a each block. Once it has read a block name, it searches for a block object to
// handle reading the remainder of the block's contents. The block object is responsible for reading the END or
// ENDBLOCK command as well as the trailing semicolon. This function also handles reading comments that are outside
// of blocks, as well as the initial "#NEXUS" keyword.
bool NexusParserReader::execute(NexusParserToken &token)
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
    catch (NexusParserException x)
    {
        logError(token.errorMessage, 0, 0, 0);
        return false;
    }

    if (!token.equals("#NEXUS")) {
        errorMessage = "Expecting \"#NEXUS\" to be the first token in the file, but found \"";
        errorMessage += token.getToken();
        errorMessage += "\" instead";
        logError(errorMessage, token.getFilePosition(), token.getFileLine(), token.getFileColumn());
        return false;
    } else {
        logMesssage("found \"#NEXUS\" as the first token in the file.");
    }

    // Start searching for blocks
    logMesssage("searching for BLOCKS...");
    for (;;)
    {
        token.setLabileFlagBit(NexusParserToken::saveCommandComments);
        token.getNextToken();

        if (token.getAtEndOfFile()){
            break;
        }

        if (token.equals("BEGIN")) {

            token.getNextToken();
            QString currentBlockName = token.getToken().toUpper();
            logMesssage(QString("looking for BLOCK reader for block called \"%1\".").arg(currentBlockName));

            // Find Block Class to reader BLOCK
            for (currentBlock = blockList; currentBlock != NULL; currentBlock = currentBlock->next)
            {
                if (token.equals(currentBlock->getID())){
                    break;
                }
            }
            logMesssage(QString("found a BLOCK called \"%1\" on line %2.").arg(currentBlockName).arg(token.getFileLine()));

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
                    } catch (NexusParserException x) {
                        if (currentBlock->errorMessage.length() > 0) {
                            logError(currentBlock->errorMessage, x.filePos, x.fileLine, x.fileCol);
                        } else {
                            logError(x.msg, x.filePos, x.fileLine, x.fileCol);
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
    logMesssage(QString("NEXUS Class Library has executed and returned with no fatal errors in %1 ms.").arg(nMilliSeconds));


    return true;
}

// Called by the Nxs object when a block named `blockName' is entered. Allows derived class overriding this
// function to notify user of progress in parsing the NEXUS file. Also gives program the opportunity to ask user if it
// is ok to purge data currently contained in this block. If user is asked whether existing data should be deleted, and
// the answer comes back no, then then the overrided function should return false, otherwise it should return true.
// This (base class) version always returns true.
bool NexusParserReader::enteringBlock(QString currentBlockName)
{
    logMesssage(QString("entering BLOCK called \"%1\"...").arg(currentBlockName));
    return true;
}

// Called by the Nxs object when a block named `blockName' is being exited. Allows derived class overriding this
// function to notify user of progress in parsing the NEXUS file.
void NexusParserReader::exitingBlock(QString currentBlockName)
{
    logMesssage(QString("exiting BLOCK called \"%1\"...").arg(currentBlockName));
}

// Called after `block' has returned from NexusParserReader::read()
void NexusParserReader::postBlockReadingHook(NexusParserBlock *block)
{
    logMesssage(QString("running post-read hook on BLOCK called \"%1\"...").arg(block->getID()));

}

void NexusParserReader::skippingBlock(QString currentBlockName)
{
    logMesssage(QString("no reader found for BLOCK called \"%1\", skipping BLOCK.").arg(currentBlockName));
}

void NexusParserReader::skippingDisabledBlock(QString currentBlockName)
{
    logMesssage(QString("BLOCK called \"%1\" is disabled, skipping BLOCK.").arg(currentBlockName));
}

bool NexusParserReader::readUntilEndblock(NexusParserToken token, QString currentBlockName)
{
    for (;;)
    {
        token.getNextToken();
        if (token.equals("END") || token.equals("ENDBLOCK")){
            logMesssage(QString("found \"END\" or \"ENDBLOCK\" for BLOCK called \"%1\" on line %2.").arg(currentBlockName).arg(token.getFileLine()));
            token.getNextToken();
            if (!token.equals(";")){
                QString errorMessage = QString("Expecting ';' after END or ENDBLOCK command, but found \"%1\" instead").arg(token.getToken());
                logError(errorMessage, token.getFilePosition(), token.getFileLine(), token.getFileColumn());
                return false;
            }
            return true;
        }
        if (token.getAtEndOfFile()){
            QString errorMessage = QString("Encountered end of file before END or ENDBLOCK in BLOCK called \"%1\".").arg(token.getToken());
            logError(errorMessage, token.getFilePosition(), token.getFileLine(), token.getFileColumn());
            return false;
        }
    }
}


// Called when an error is encountered in a NEXUS file. Allows program to give user details of the error as well as
// the precise location of the error via the application log.
void NexusParserReader::logError(QString message, qint64 filePos, qint64 fileLine, qint64 fileCol)
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
void NexusParserReader::logMesssage(QString message)
{
    // Write to main window application log
    mainwindow->logAppend("NEXUS Reader",QString("%1").arg(message));
}
