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

#include "nexusreadertaxablock.h"
#include "nexusreaderexception.h"

NexusReaderTaxaBlock::NexusReaderTaxaBlock()
{
    taxaNumber = 0;
    blockID = "TAXA";
}

NexusReaderTaxaBlock::~NexusReaderTaxaBlock()
{
    taxonLabels.clear();
}

// This function provides the ability to read everything following the block name (which is read by the NexusReader
// object) to the end or endblock statement. Characters are read from the input stream in. Overrides the abstract
// virtual function in the base class.
void NexusReaderTaxaBlock::read(NexusReaderToken *&token)
{
    taxaNumber = 0;
    int nominalTaxaNumber = 0;
    isEmpty = false;

    requireSemicolonToken(token, "BEGIN TAXA");

    for (;;)
    {
        token->getNextToken();
        NexusReaderBlock::NexusCommandResult result = handleBasicBlockCommands(token);

        if (result == NexusReaderBlock::NexusCommandResult(STOP_PARSING_BLOCK)){
            return;
        }
        if (result != NexusReaderBlock::NexusCommandResult(HANDLED_COMMAND)){
            if (token->tokenEquals("DIMENSIONS")){
                nexusReader->nexusReaderLogMesssage(QString("TAXA Block: found command \"DIMENSIONS\" on line %1, now looking for \"NTAX\"...").arg(token->getFileLine()));
                // This should be NTAX keyword
                token->getNextToken();
                if (!token->tokenEquals("NTAX")){
                    errorMessage = "Expecting NTAX keyword, but found ";
                    errorMessage += token->getToken();
                    errorMessage += " instead";
                    throw NexusReaderException(errorMessage, token->getFilePosition(), token->getFileLine(), token->getFileColumn());
                }
                requireEqualsToken(token, "after NTAX");
                nominalTaxaNumber = requirePositiveToken(token, "NTAX");
                nexusReader->nexusReaderLogMesssage(QString("TAXA Block: found command \"NTAX\" on line %2. Taxa Number = %1. Now looking for \"TAXLABELS\"...").arg(nominalTaxaNumber).arg(token->getFileLine()));
                requireSemicolonToken(token, "DIMENSIONS");
            } else if (token->tokenEquals("TAXLABELS")) {
                qDebug() << "TAXLABELS found";
                if (nominalTaxaNumber <= 0) {
                    throw NexusReaderException("NTAX must be specified before TAXLABELS command", token->getFilePosition(), token->getFileLine(), token->getFileColumn());
                }

                nexusReader->nexusReaderLogMesssage(QString("TAXA Block: found command \"TAXLABELS\" on line %1, now extracting taxon labels...").arg(token->getFileLine()));

                for (int i = 0; i < nominalTaxaNumber; i++)
                {
                    token->getNextToken();
                    // Should check to make sure this is not punctuation
                    addTaxonLabel(token->getToken(), token->needsQuotes(token->getToken()));
                    nexusReader->nexusReaderLogMesssage(QString("TAXA Block: extracted taxon label [T%1] -> %2.").arg(i+1).arg(token->getToken()));
                }
                requireSemicolonToken(token, "TAXLABELS");
            } else {
                skippingCommand(token->getToken());
                do {
                    token->getNextToken();
                } while (!token->getAtEndOfFile() && !token->tokenEquals(";"));

                if (token->getAtEndOfFile()){
                    errorMessage = "Unexpected end of file encountered";
                    throw NexusReaderException(errorMessage, token->getFilePosition(), token->getFileLine(), token->getFileColumn());
                }
            }
        }
    }
}

// Adds taxon label to end of list of taxon labels and increments ntax by 1. Returns index of taxon label just
// added.
int NexusReaderTaxaBlock::addTaxonLabel(QString taxonLabel, bool quotesNeeded)
{
    isEmpty = false;
    needsQuotes.append(quotesNeeded);
    taxonLabels.append(taxonLabel);
    taxaNumber++;
    return (taxaNumber-1);
}

// Flushes taxonLabels and sets taxaNumber to 0 in preparation for reading a new TAXA block.
void NexusReaderTaxaBlock::reset()
{
    NexusReaderBlock::reset();
    taxaNumber = 0;
    taxonLabels.clear();
    needsQuotes.clear();
}
