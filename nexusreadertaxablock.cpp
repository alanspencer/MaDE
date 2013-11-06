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

NexusReaderTaxaBlock::NexusReaderTaxaBlock()
{
    ntax = 0;
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
    ntax = 0;
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
            if (token->equals("DIMENSIONS")){
                nominalTaxaNumber = handleDimensions(token, "NTAX");
            } else if (token->equals("TAXLABELS")) {
                if (nominalTaxaNumber <= 0) {
                    throw NexusReaderException("NTAX must be specified before TAXLABELS command", token->getFilePosition(), token->getFileLine(), token->getFileColumn());
                }

                nexusReader->nexusReaderLogMesssage(QString("TAXA Block: found command \"TAXLABELS\" on line %1, now extracting taxon labels...").arg(token->getFileLine()));

                for (int i = 0; i < nominalTaxaNumber; i++)
                {
                    token->getNextToken();
                    // Should check to make sure this is not punctuation
                    addTaxonLabel(token->getToken());
                    nexusReader->nexusReaderLogMesssage(QString("TAXA Block: extracted taxon label [T%1] -> %2.").arg(i+1).arg(token->getToken()));
                }
                requireSemicolonToken(token, "TAXLABELS");
            } else {
                skippingCommand(token->getToken());
                do {
                    token->getNextToken();
                } while (!token->getAtEndOfFile() && !token->equals(";"));

                if (token->getAtEndOfFile()){
                    errorMessage = "Unexpected end of file encountered";
                    throw NexusReaderException(errorMessage, token->getFilePosition(), token->getFileLine(), token->getFileColumn());
                }
            }
        }
    }
}

// Called when DIMENSIONS command needs to be parsed from within the TAXA block. Deals with everything after the token DIMENSIONS up
// to and including the semicolon that terminates the DIMENSIONs command. `ntaxLabel' is simply "NTAX" for this class, but may be
// different for derived classes that use `ntax' for other things.
int NexusReaderTaxaBlock::handleDimensions(NexusReaderToken *&token, QString ntaxLabel)
{
    int nominalTaxaNumber = 0;
    nexusReader->nexusReaderLogMesssage(
                QString("TAXA Block: found command \"DIMENSIONS\" on line %1, now looking for \"%2\"...")
                .arg(token->getFileLine())
                .arg(ntaxLabel)
                );

    // This should be NTAX keyword
    token->getNextToken();
    if (!token->equals(ntaxLabel)){
        errorMessage = QString("Expecting %1 keyword, but found %2 instead.").arg(ntaxLabel).arg(token->getToken());
        throw NexusReaderException(errorMessage, token->getFilePosition(), token->getFileLine(), token->getFileColumn());
    }
    requireEqualsToken(token, QString("after %1").arg(ntaxLabel));
    nominalTaxaNumber = requirePositiveToken(token, ntaxLabel);

    nexusReader->nexusReaderLogMesssage(
                QString("TAXA Block: found command \"%3\" on line %2. NTAX = %1. Now looking for \"TAXLABELS\"...")
                .arg(nominalTaxaNumber)
                .arg(token->getFileLine())
                .arg(ntaxLabel)
                );

    requireSemicolonToken(token, "DIMENSIONS");
    return nominalTaxaNumber;
}

// Adds taxon label to end of list of taxon labels and increments ntax by 1. Returns index of taxon label just
// added.
int NexusReaderTaxaBlock::addTaxonLabel(QString taxonLabel)
{
    isEmpty = false;
    taxonLabels.append(taxonLabel);
    ntax++;
    return (ntax-1);
}

int NexusReaderTaxaBlock::getNTAX()
{
    return ntax;
}

// This virtual function must be overridden for each derived class to provide the ability to return a standard data object.
QMap<QString, QVariant> NexusReaderTaxaBlock::getData()
{
    blockData.insert("NTAX", ntax);
    blockData.insert("TAXLABELS", taxonLabels);
    return blockData;
}

// Flushes taxonLabels and sets taxaNumber to 0 in preparation for reading a new TAXA block.
void NexusReaderTaxaBlock::reset()
{
    NexusReaderBlock::reset();
    ntax = 0;
    taxonLabels.clear();
}
