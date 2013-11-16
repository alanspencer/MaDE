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

NexusParserCharactersBlock::NexusParserCharactersBlock(NexusParserReader *pointer, NexusParserTaxaBlock *tBlock)
{
    Q_ASSERT(tBlock != NULL);
    //assert(aBlock != NULL);

    setNexusParserReader(pointer);
    blockID = "CHARACTERS";
    taxaBlock = tBlock;

    reset();
}

NexusParserCharactersBlock::~NexusParserCharactersBlock()
{
    reset();

    if(symbols.count() > 0) {
        symbols.clear();
    }
}

// Reset
void NexusParserCharactersBlock::reset()
{
    NexusParserBlock::reset();

    ncharTotal = 0;
    nchar = 0;
    nextCharacterID = 0;
    characterList.clear();
    newchar = true;

    ntaxTotal = 0;
    ntax = 0;    
    newtaxa = false;

    interleaving = false;
    transposing = false;
    respectingCase = false;
    labels = true;
    tokens = false;
    datatype = NexusParserCharactersBlock::standard;

    missing = nexusParser->defaultMissingCharacter;
    gap = nexusParser->defaultGapCharacter;
    matchchar = nexusParser->defaultMatchCharacter;

    resetSymbols();
    equatesList.clear();

    items.clear();
    items.append("STATES");

    statesFormat = STATES_PRESENT;
}

/*------------------------------------------------------------------------------------/
 * Read Function
 *-----------------------------------------------------------------------------------*/

// This function provides the ability to read everything following the block name (which is read by the NexusParserReader
// object) to the END or ENDBLOCK statement. Characters are read from the input stream `in'. Overrides the abstract
// virtual function in the base class.
void NexusParserCharactersBlock::read(NexusParserToken &token)
{
    isEmpty = false;
    demandEndSemicolon(token, QString("BEGIN %1").arg(blockID));

    // Get Taxa Number (nTax)
    ntax = taxaBlock->getNumTaxonLabels();
    for(;;)
    {
        token.getNextToken();
        NexusParserBlock::NexusParserCommandResult result = handleBasicBlockCommands(token);
        if (result == NexusParserBlock::NexusParserCommandResult(STOP_PARSING_BLOCK)){
            return;
        }
        if (result != NexusParserBlock::NexusParserCommandResult(HANDLED_COMMAND)) {
            if (token.equals("DIMENSIONS")) {
                handleDimensions(token, "NEWTAXA", "NTAX", "NCHAR");
            } else if (token.equals("FORMAT")) {
                handleFormat(token);
            } else if (token.equals("ELIMINATE")) {
                handleEliminate(token);
            } else if (token.equals("TAXLABELS")) {
                handleTaxlabels(token);
            } else if (token.equals("CHARSTATELABELS")) {
                handleCharstatelabels(token);
            } else if (token.equals("CHARLABELS")) {
                handleCharlabels(token);
            } else if (token.equals("STATELABELS")) {
                handleStatelabels(token);
            } else if (token.equals("MATRIX")) {
                handleMatrix(token);
            } else {
                skippingCommand(token.getToken());
                do {
                    token.getNextToken();
                } while (!token.getAtEndOfFile() && !token.equals(";"));

                if (token.getAtEndOfFile()){
                    errorMessage = "Quick, hide under the desk we have a problem! Unexpected end of file encountered";
                    throw NexusParserException(errorMessage,
                                       token.getFilePosition(),
                                       token.getFileLine(),
                                       token.getFileColumn());
                }
            }
        }
    }
}

/*------------------------------------------------------------------------------------/
 * handleXXXXXX Functions
 *-----------------------------------------------------------------------------------*/

// Called when DIMENSIONS command needs to be parsed from within the CHARACTERS block. Deals with everything after
// the token DIMENSIONS up to and including the semicolon that terminates the DIMENSIONs command. `newtaxaLabel',
// `ntaxLabel' and `ncharLabel' are simply "NEWTAXA", "NTAX" and "NCHAR" for this class, but may be different for
// derived classes that use `newtaxa', `ntax' and `nchar' for other things (e.g., ntax is number of populations in
// an ALLELES block)
void NexusParserCharactersBlock::handleDimensions(NexusParserToken &token, QString newtaxaLabel, QString ntaxLabel, QString ncharLabel)
{
    nexusParser->logMesssage(
                QString("%1 BLOCK: found command \"DIMENSIONS\" on line %2, now looking for \"%3\" or \"%4\" or \"%5\"...")
                .arg(blockID)
                .arg(token.getFileLine())
                .arg(newtaxaLabel)
                .arg(ntaxLabel)
                .arg(ncharLabel)
                );
    for (;;)
    {
        token.getNextToken();

        if (token.equals(newtaxaLabel)) {
            newtaxa = true;

            nexusParser->logMesssage(
                        QString("%1 BLOCK: found subcommand \"%2\" on line %3.")
                        .arg(blockID)
                        .arg(newtaxaLabel)
                        .arg(token.getFileLine())
                        );
        } else if (token.equals(ntaxLabel)){
            demandEquals(token, "in DIMENSIONS command");
            ntax = demandPositiveInt(token, ntaxLabel);

            nexusParser->logMesssage(
                        QString("%1 BLOCK: found subcommand \"%2\" on line %3. NTAX = %4.")
                        .arg(blockID)
                        .arg(ntaxLabel)
                        .arg(token.getFileLine())
                        .arg(ntax)
                        );

            if (newtaxa) {
                ntaxTotal = ntax;
            } else {
                ntaxTotal = taxaBlock->getNumTaxonLabels();
                if (ntaxTotal < ntax){
                    errorMessage = ntaxLabel;
                    errorMessage += " in ";
                    errorMessage += blockID;
                    errorMessage += " block must be less than or equal to NTAX in TAXA block\nNote: one circumstance that can cause this error is \nforgetting to specify ";
                    errorMessage += ntaxLabel;
                    errorMessage += " in DIMENSIONS command when \na TAXA block has not been provided";
                    throw NexusParserException(errorMessage,
                                       token.getFilePosition(),
                                       token.getFileLine(),
                                       token.getFileColumn());
                }
            }
        } else if (token.equals(ncharLabel)){
            demandEquals(token, "in DIMENSIONS command");
            nchar = demandPositiveInt(token, ncharLabel);
            ncharTotal = nchar;

            nexusParser->logMesssage(
                        QString("%1 BLOCK: found subcommand \"%2\" on line %3. NCHAR = %4.")
                        .arg(blockID)
                        .arg(ncharLabel)
                        .arg(token.getFileLine())
                        .arg(nchar)
                        );
        } else if (token.equals(";")){
            break;
        }
    }

    if (newtaxa) {
        taxaBlock->reset();
    }
}

// Called when FORMAT command needs to be parsed from within the CHARACTERS block. Deals with everything after the
// token FORMAT up to and including the semicolon that terminates the FORMAT command.
void NexusParserCharactersBlock::handleFormat(NexusParserToken &token)
{
    nexusParser->logMesssage(
                QString("%1 BLOCK: found command \"FORMAT\" on line %2...")
                .arg(blockID)
                .arg(token.getFileLine())
                );

    bool standardDataTypeAssumed = false;
    bool ignoreCaseAssumed = false;

    for (;;)
    {
        token.getNextToken();

        if (token.equals("DATATYPE")){
            demandEquals(token, "after keyword DATATYPE");

            // This should be one of the following: STANDARD, DNA, RNA, NUCLEOTIDE, PROTEIN, or CONTINUOUS
            token.getNextToken();

            if (token.equals("STANDARD")) {
                datatype = standard;
            } else if (token.equals("DNA")) {
                datatype = dna;
            } else if (token.equals("RNA")) {
                datatype = rna;
            } else if (token.equals("NUCLEOTIDE")) {
                datatype = nucleotide;
            } else if (token.equals("PROTEIN")) {
                datatype = protein;
            } else if (token.equals("CONTINUOUS")) {
                datatype = continuous;
                statesFormat = INDIVIDUALS;
                items.append("AVERAGE");
            } else {
                errorMessage = token.getToken();
                errorMessage += " is not a valid DATATYPE within a ";
                errorMessage += blockID;
                errorMessage += " block";
                throw NexusParserException(errorMessage,
                                   token.getFilePosition(),
                                   token.getFileLine(),
                                   token.getFileColumn());
            }

            nexusParser->logMesssage(
                        QString("%1 BLOCK: found subcommand \"DATATYPE\" on line %2. DATATYPE = %3.")
                        .arg(blockID)
                        .arg(token.getFileLine())
                        .arg(token.getToken())
                        );

            if (standardDataTypeAssumed && datatype != standard) {
                throw NexusParserException("DATATYPE must be specified first in FORMAT command",
                                   token.getFilePosition(),
                                   token.getFileLine(),
                                   token.getFileColumn());
            }

            resetSymbols();

            if (datatype == continuous) {
                tokens = true;
            }

        } else if (token.equals("RESPECTCASE")) {
            if (ignoreCaseAssumed) {
                throw NexusParserException("RESPECTCASE must be specified before MISSING, GAP, SYMBOLS, and MATCHCHAR in FORMAT command",
                                   token.getFilePosition(),
                                   token.getFileLine(),
                                   token.getFileColumn());
            }

            nexusParser->logMesssage(
                        QString("%1 BLOCK: found subcommand \"RESPECTCASE\" on line %2.")
                        .arg(blockID)
                        .arg(token.getFileLine())
                        );

            standardDataTypeAssumed = true;
            respectingCase = true;
        } else if (token.equals("MISSING")) {
            demandEquals(token, "after keyword MISSING");

            // This should be the missing data symbol (single character)
            token.getNextToken();

            if (token.getTokenLength() != 1){
                errorMessage = "MISSING symbol should be a single character, but ";
                errorMessage += token.getToken();
                errorMessage += " was specified";
                throw NexusParserException(errorMessage,
                                   token.getFilePosition(),
                                   token.getFileLine(),
                                   token.getFileColumn());
            } else if (token.isPunctuationToken() && !token.isPlusMinusToken()){
                errorMessage = "MISSING symbol specified cannot be a punctuation token (";
                errorMessage += token.getToken();
                errorMessage += " was specified)";
                throw NexusParserException(errorMessage,
                                   token.getFilePosition(),
                                   token.getFileLine(),
                                   token.getFileColumn());
            } else if (token.isWhitespaceToken()) {
                errorMessage = "MISSING symbol specified cannot be a whitespace character (";
                errorMessage += token.getToken();
                errorMessage += " was specified)";
                throw NexusParserException(errorMessage,
                                   token.getFilePosition(),
                                   token.getFileLine(),
                                   token.getFileColumn());
            }

            missing = token.getToken().at(0);

            nexusParser->logMesssage(
                        QString("%1 BLOCK: found subcommand \"MISSING\" on line %2. MISSING = %3.")
                        .arg(blockID)
                        .arg(token.getFileLine())
                        .arg(missing)
                        );

            ignoreCaseAssumed = true;
            standardDataTypeAssumed = true;
        } else if (token.equals("GAP")) {
            demandEquals(token, "after keyword GAP");

            // This should be the gap symbol (single character)
            token.getNextToken();

            if (token.getTokenLength() != 1) {
                errorMessage = "GAP symbol should be a single character, but ";
                errorMessage += token.getToken();
                errorMessage += " was specified";
                throw NexusParserException(errorMessage,
                                   token.getFilePosition(),
                                   token.getFileLine(),
                                   token.getFileColumn());
            } else if (token.isPunctuationToken() && !token.isPlusMinusToken()) {
                errorMessage = "GAP symbol specified cannot be a punctuation token (";
                errorMessage += token.getToken();
                errorMessage += " was specified)";
                throw NexusParserException(errorMessage,
                                   token.getFilePosition(),
                                   token.getFileLine(),
                                   token.getFileColumn());
            }  else if (token.isWhitespaceToken()) {
                errorMessage = "GAP symbol specified cannot be a whitespace character (";
                errorMessage += token.getToken();
                errorMessage += " was specified)";
                throw NexusParserException(errorMessage,
                                   token.getFilePosition(),
                                   token.getFileLine(),
                                   token.getFileColumn());
            }

            gap = token.getToken().at(0);

            nexusParser->logMesssage(
                        QString("%1 BLOCK: found subcommand \"GAP\" on line %2. GAP = %3.")
                        .arg(blockID)
                        .arg(token.getFileLine())
                        .arg(gap)
                        );

            ignoreCaseAssumed = true;
            standardDataTypeAssumed = true;
        } else if (token.equals("SYMBOLS")) {
            if (datatype == NexusParserCharactersBlock::continuous) {
                throw NexusParserException("SYMBOLS subcommand not allowed for DATATYPE=CONTINUOUS",
                                   token.getFilePosition(),
                                   token.getFileLine(),
                                   token.getFileColumn());
            }

            int numDefStates;
            int maxNewStates;
            switch(datatype){
                case NexusParserCharactersBlock::dna:
                case NexusParserCharactersBlock::rna:
                case NexusParserCharactersBlock::nucleotide:
                    numDefStates = 4;
                    maxNewStates = NEXUS_MAX_STATES-numDefStates;
                    break;

                case NexusParserCharactersBlock::protein:
                    numDefStates = 21;
                    maxNewStates = NEXUS_MAX_STATES-numDefStates;
                    break;

                default:
                    numDefStates = 0;
                    symbols.clear();
                    maxNewStates = NEXUS_MAX_STATES-numDefStates;
                    break;
            }

            demandEquals(token, " after keyword SYMBOLS");

            // This should be the symbols list
            token.setLabileFlagBit(NexusParserToken::doubleQuotedToken);
            token.getNextToken();
            token.stripWhitespace();

            int numNewSymbols = token.getTokenLength();

            if (numNewSymbols > maxNewStates){
                errorMessage = "SYMBOLS defines ";
                errorMessage += numNewSymbols;
                errorMessage += " new states but only ";
                errorMessage += maxNewStates;
                errorMessage += " new states allowed for this DATATYPE";
                throw NexusParserException(errorMessage,
                                   token.getFilePosition(),
                                   token.getFileLine(),
                                   token.getFileColumn());
            }

            QString t = token.getToken();
            int tlen = t.size();

            // Check to make sure user has not used any symbols already in the default symbols list for this data type
            for (int i = 0; i < tlen; i++)
            {
                if (isInSymbols(t.at(i))){
                    errorMessage = "The character ";
                    errorMessage += t[i];
                    errorMessage += " defined in SYMBOLS has already been predefined for this DATATYPE";
                    throw NexusParserException(errorMessage,
                                       token.getFilePosition(),
                                       token.getFileLine(),
                                       token.getFileColumn());
                }
            }

            // If we've made it this far, go ahead and add the user-defined symbols to the end of the list of predefined symbols
            QString symbolsString;
            for (int i = 0; i < tlen; i++)
            {
                symbols.append(t.at(i));
                symbolsString.append(t.at(i));
            }

            nexusParser->logMesssage(
                        QString("%1 BLOCK: found subcommand \"SYMBOLS\" on line %2. SYMBOLS = %3.")
                        .arg(blockID)
                        .arg(token.getFileLine())
                        .arg(symbolsString)
                        );

            ignoreCaseAssumed = true;
            standardDataTypeAssumed = true;
        } else if (token.equals("EQUATE")) {

            if (datatype == NexusParserCharactersBlock::continuous) {
                throw NexusParserException("EQUATE subcommand not allowed for DATATYPE=CONTINUOUS",
                                   token.getFilePosition(),
                                   token.getFileLine(),
                                   token.getFileColumn());
            }

            demandEquals(token, "after keyword EQUATE");

            // This should be a double-quote character
            token.getNextToken();

            if (!token.equals("\"")) {
                errorMessage = "Expecting '\"' after keyword EQUATE but found ";
                errorMessage += token.getToken();
                errorMessage += " instead";
                throw NexusParserException(errorMessage,
                                   token.getFilePosition(),
                                   token.getFileLine(),
                                   token.getFileColumn());
            }

            // Loop until second double-quote character is encountered
            for (;;)
            {
                token.getNextToken();
                if (token.equals("\"")) {
                    break;
                }

                // If token is not a double-quote character, then it must be the equate symbol (i.e., the
                // character to be replaced in the data matrix)
                if (token.getTokenLength() != 1)
                {
                    errorMessage = "Expecting single-character EQUATE symbol but found ";
                    errorMessage += token.getToken();
                    errorMessage += " instead";
                    throw NexusParserException(errorMessage,
                                       token.getFilePosition(),
                                       token.getFileLine(),
                                       token.getFileColumn());
                }

                // Check for bad choice of equate symbol
                QString t = token.getToken();
                QChar ch = t.at(0);
                bool badEquateSymbol = false;

                // The character '^' cannot be an equate symbol
                if (ch == QChar('^')) {
                    badEquateSymbol = true;
                }

                // Equate symbols cannot be punctuation (except for + and -)
                if (token.isPunctuationToken() && !token.isPlusMinusToken()){
                    badEquateSymbol = true;
                }

                // Equate symbols cannot be same as matchchar, missing, or gap
                if (ch == missing || ch == matchchar || ch == gap){
                    badEquateSymbol = true;
                }

                // Equate symbols cannot be one of the state symbols currently defined
                if (isInSymbols(ch)){
                    badEquateSymbol = true;
                }

                if (badEquateSymbol){
                    errorMessage = "EQUATE symbol specified (";
                    errorMessage += token.getToken();
                    errorMessage += ") is not valid; must not be same as missing, matchchar, gap, state symbols, or any of the following: ()[]{}/\\,;:=*'\"`<>^";
                    throw NexusParserException(errorMessage,
                                       token.getFilePosition(),
                                       token.getFileLine(),
                                       token.getFileColumn());
                }

                QString symbol = token.getToken();
                demandEquals(token, " in EQUATE definition");

                // This should be the token to be substituted in for the equate symbol
                token.setLabileFlagBit(NexusParserToken::parentheticalToken);
                token.setLabileFlagBit(NexusParserToken::curlyBracketedToken);
                token.getNextToken();
                QString equivalent = token.getToken();

                // Add the new equate association to the equates list
                equateAdd(symbol, equivalent);
            }

            nexusParser->logMesssage(
                        QString("%1 BLOCK: found subcommand \"EQUATE\" on line %2.")
                        .arg(blockID)
                        .arg(token.getFileLine())
                        );

            standardDataTypeAssumed = true; 
        } else if (token.equals("MATCHCHAR")) {
            demandEquals(token, "after keyword MATCHCHAR");

            // This should be the matchchar symbol (single character)
            token.getNextToken();

            if (token.getTokenLength() != 1){
                errorMessage = "MATCHCHAR symbol should be a single character, but ";
                errorMessage += token.getToken();
                errorMessage += " was specified";
                throw NexusParserException(errorMessage,
                                   token.getFilePosition(),
                                   token.getFileLine(),
                                   token.getFileColumn());
            } else if (token.isPunctuationToken() && !token.isPlusMinusToken()){
                errorMessage = "MATCHCHAR symbol specified cannot be a punctuation token (";
                errorMessage += token.getToken();
                errorMessage += " was specified) ";
                throw NexusParserException(errorMessage,
                                   token.getFilePosition(),
                                   token.getFileLine(),
                                   token.getFileColumn());
            } else if (token.isWhitespaceToken()){
                errorMessage = "MATCHCHAR symbol specified cannot be a whitespace character (";
                errorMessage += token.getToken();
                errorMessage += " was specified)";
                throw NexusParserException(errorMessage,
                                   token.getFilePosition(),
                                   token.getFileLine(),
                                   token.getFileColumn());
            }

            matchchar = token.getToken().at(0);

            nexusParser->logMesssage(
                        QString("%1 BLOCK: found subcommand \"MATCHCHAR\" on line %2. MATCHCHAR = %3.")
                        .arg(blockID)
                        .arg(token.getFileLine())
                        .arg(matchchar)
                        );

            ignoreCaseAssumed = true;
            standardDataTypeAssumed = true;
        } else if (token.equals("LABELS")) {
            labels = true;

            nexusParser->logMesssage(
                        QString("%1 BLOCK: found subcommand \"LABELS\" on line %2.")
                        .arg(blockID)
                        .arg(token.getFileLine())
                        );

            standardDataTypeAssumed = true;
        } else if (token.equals("NOLABELS")) {
            labels = false;

            nexusParser->logMesssage(
                        QString("%1 BLOCK: found subcommand \"NOLABELS\" on line %2.")
                        .arg(blockID)
                        .arg(token.getFileLine())
                        );

            standardDataTypeAssumed = true;
        } else if (token.equals("TRANSPOSE")) {
            transposing = true;

            nexusParser->logMesssage(
                        QString("%1 BLOCK: found subcommand \"TRANSPOSE\" on line %2.")
                        .arg(blockID)
                        .arg(token.getFileLine())
                        );

            standardDataTypeAssumed = true;
        } else if (token.equals("INTERLEAVE")) {
            interleaving = true;

            nexusParser->logMesssage(
                        QString("%1 BLOCK: found subcommand \"INTERLEAVE\" on line %2.")
                        .arg(blockID)
                        .arg(token.getFileLine())
                        );

            standardDataTypeAssumed = true;
        } else if (token.equals("ITEMS")){
            demandEquals(token, "after keyword ITEMS");
            items.clear();

            // This should be STATES (no other item is supported at this time)
            token.getNextToken();
            if (datatype == NexusParserCharactersBlock::continuous){
                QString str;
                if (token.equals("(")){
                    token.getNextToken();
                    while (!token.equals(")")){
                        str = token.getToken().toUpper();
                        items.append(str);
                        token.getNextToken();
                    }
                } else {
                    str = token.getToken().toUpper();
                    items.append(str);
                }
            } else {
                if (!token.equals("STATES")) {
                    throw NexusParserException("Sorry, only ITEMS=STATES is supported for discrete datatypes at this time",
                                       token.getFilePosition(),
                                       token.getFileLine(),
                                       token.getFileColumn());
                }
                items.clear();
                items.append("STATES");
            }

            nexusParser->logMesssage(
                        QString("%1 BLOCK: found subcommand \"ITEMS\" on line %2.")
                        .arg(blockID)
                        .arg(token.getFileLine())
                        );

            standardDataTypeAssumed = true;
        } else if (token.equals("STATESFORMAT")) {
            demandEquals(token, "after keyword STATESFORMAT");

            // This should be STATESPRESENT (no other statesformat is supported at this time)
            token.getNextToken();

            if (token.equals("STATESPRESENT")) {
                statesFormat = STATES_PRESENT;
            } else {
                if (datatype == NexusParserCharactersBlock::continuous) {
                    if (token.equals("INDIVIDUALS")) {
                        statesFormat = INDIVIDUALS;
                    } else {
                        throw NexusParserException("Sorry, only STATESFORMAT=STATESPRESENT or STATESFORMAT=INDIVIDUALS are supported for continuous datatypes at this time",
                                           token.getFilePosition(),
                                           token.getFileLine(),
                                           token.getFileColumn());
                    }
                } else {
                    throw NexusParserException("Sorry, only STATESFORMAT=STATESPRESENT supported for discrete datatypes at this time",
                                       token.getFilePosition(),
                                       token.getFileLine(),
                                       token.getFileColumn());
                }
            }

            nexusParser->logMesssage(
                        QString("%1 BLOCK: found subcommand \"STATESFORMAT\" on line %2.")
                        .arg(blockID)
                        .arg(token.getFileLine())
                        );

            standardDataTypeAssumed = true;
        } else if (token.equals("TOKENS")) {
            tokens = true;

            nexusParser->logMesssage(
                        QString("%1 BLOCK: found subcommand \"TOKENS\" on line %2.")
                        .arg(blockID)
                        .arg(token.getFileLine())
                        );

            standardDataTypeAssumed = true;
        } else if (token.equals("NOTOKENS")) {
            if (datatype == NexusParserCharactersBlock::continuous) {
                throw NexusParserException("NOTOKENS is not allowed for the CONTINUOUS datatype",
                                   token.getFilePosition(),
                                   token.getFileLine(),
                                   token.getFileColumn());
            }
            tokens = false;

            nexusParser->logMesssage(
                        QString("%1 BLOCK: found subcommand \"NOTOKENS\" on line %2.")
                        .arg(blockID)
                        .arg(token.getFileLine())
                        );

            standardDataTypeAssumed = true;
        } else if (token.equals(";")) {
            break;
        }
    }

    // Perform some last checks before leaving the FORMAT command
    if (!tokens && datatype == continuous) {
        throw NexusParserException("TOKENS must be defined for DATATYPE=CONTINUOUS",
                           token.getFilePosition(),
                           token.getFileLine(),
                           token.getFileColumn());
    }
    if (tokens && (datatype == dna || datatype == rna || datatype == nucleotide)) {
        throw NexusParserException("TOKENS not allowed for the DATATYPEs DNA, RNA, or NUCLEOTIDE",
                           token.getFilePosition(),
                           token.getFileLine(),
                           token.getFileColumn());
    }
}

// Called when ELIMINATE command needs to be parsed from within the CHARACTERS block. Deals with everything after the
// token ELIMINATE up to and including the semicolon that terminates the ELIMINATE command. Any character numbers
// or ranges of character numbers specified are stored in the NxsUnsignedSet `eliminated', which remains empty until
// an ELIMINATE command is processed. Note that like all sets the character ranges are adjusted so that their offset
// is 0. For example, given "eliminate 4-7;" in the data file, the eliminate array would contain the values 3, 4, 5
// and 6 (not 4, 5, 6 and 7). It is assumed that the ELIMINATE command comes before character labels and/or character
// state labels have been specified; an error message is generated if the user attempts to use ELIMINATE after a
// CHARLABELS, CHARSTATELABELS, or STATELABELS command.
void NexusParserCharactersBlock::handleEliminate(NexusParserToken &token)
{
    nexusParser->logMesssage(
                QString("%1 BLOCK: found command \"ELIMINATE\" on line %2...")
                .arg(blockID)
                .arg(token.getFileLine())
                );

    // Construct an object of type NexusParserSetReader, then call its run function
    // to store the set in the eliminated set
    NexusParserSetReader setReader(token, ncharTotal, eliminated, *this, NexusParserSetReader::charset);
    setReader.run();

    Q_ASSERT(eliminated.count() <= ncharTotal);

    nchar = ncharTotal - eliminated.count();

    if (nchar != ncharTotal && (characterList.count() > 0)) {
        throw NexusParserException("The ELIMINATE command must appear before character (or character state) labels are specified", token.getFilePosition(), token.getFileLine(), token.getFileColumn());
    }

    if (!charPos.isEmpty()) {
        throw NexusParserException("Only one ELIMINATE command is allowed, and it must appear before the MATRIX command", token.getFilePosition(), token.getFileLine(), token.getFileColumn());
    }

    // Creates a charPos Map containing all characters (thoses that have been eliminated have a value of INT_MAX)
    buildCharPosArray(true);
}

// Called when TAXLABELS command needs to be parsed from within the CHARACTERS block. Deals with everything after the
// token TAXLABELS up to and including the semicolon that terminates the TAXLABELS command.
void NexusParserCharactersBlock::handleTaxlabels(NexusParserToken &token)
{
    nexusParser->logMesssage(
                QString("%1 BLOCK: found command \"TAXLABELS\" on line %2...")
                .arg(blockID)
                .arg(token.getFileLine())
                );

    if (!newtaxa){
        errorMessage = "NEWTAXA must have been specified in DIMENSIONS command to use the TAXLABELS command in a ";
        errorMessage += blockID;
        errorMessage += " block";
        throw NexusParserException(errorMessage,
                           token.getFilePosition(),
                           token.getFileLine(),
                           token.getFileColumn());
    }

    for (;;)
    {
        token.getNextToken();

        // Token should either be ';' or the name of a taxon
        if (token.equals(";")){
            break;
        } else {
            // Check to make sure user is not trying to read in more
            // taxon labels than there are taxa
            if (taxaBlock->getNumTaxonLabels() > ntaxTotal){
                throw NexusParserException("Number of taxon labels exceeds NTAX specified in DIMENSIONS command",
                                   token.getFilePosition(),
                                   token.getFileLine(),
                                   token.getFileColumn());
            }
            // Add to NexusParserTaxaBlock Class
            taxaBlock->taxonAdd(token.getToken());
        }
    }

    // NB: Some may object to setting newtaxa to false here, because then the fact that new taxa were
    // specified in this CHARACTERS block rather than in a preceding TAXA block is lost. This will only be
    // important if we wish to recreate the original data file, which I don't anticipate anyone doing with
    // this code (too difficult to remember all comments, the order of blocks in the file, etc.) - better to
    // get a copy of the file data from the QString NexusParserToken::nexusData and output it somewhere.
    newtaxa = false;
}

// Called when CHARSTATELABELS command needs to be parsed from within the CHARACTERS block. Deals with everything
// after the token CHARSTATELABELS up to and including the semicolon that terminates the CHARSTATELABELS command.
// Resulting `charLabels' list will store labels only for characters that have not been eliminated, and likewise for
// `charStates'. Specifically, `charStates[0]' refers to the vector of character state labels for the first
// non-eliminated character.
void NexusParserCharactersBlock::handleCharstatelabels(NexusParserToken &token)
{
    nexusParser->logMesssage(
                QString("%1 BLOCK: found command \"CHARSTATELABELS\" on line %2...")
                .arg(blockID)
                .arg(token.getFileLine())
                );

    int currChar = 0;
    bool semicolonFoundInInnerLoop = false;
    bool tokenAlreadyRead = false;

    characterList.clear();

    // If there has been no ELIMINATE command build a new charPos map - will be a simple map where key === value.
    if (charPos.isEmpty()){
        buildCharPosArray();
    }

    for (;;) // loop #1
    {
        // End of CHARSTATELABELS found therefore break loop.
        if (semicolonFoundInInnerLoop){
            break;
        }

        // Get next token if needed
        if (tokenAlreadyRead) {
            tokenAlreadyRead = false;
        } else {
            token.getNextToken();
        }

        // End of CHARSTATELABELS found therefore break loop.
        if (token.equals(";")) {
            break;
        }

        // Token should be the character number (i.e. 1... ncharTotal); create a new association
        int n = token.getToken().toInt();

        if (n < 1 || n > ncharTotal || n <= currChar) {
            errorMessage = "Invalid character number (";
            errorMessage += token.getToken();
            errorMessage += ") found in CHARSTATELABELS command (either out of range or not interpretable as an integer)";
            throw NexusParserException(errorMessage,
                               token.getFilePosition(),
                               token.getFileLine(),
                               token.getFileColumn());
        }

        // If n is not the next character after currChar, need to add some dummy labels to characterList to fill the gap
        while (n - currChar > 1) {
            currChar++;
            QString characterLabel = QString("Character %1").arg(n);
            if (!isEliminated(currChar - 1)) {
                characterAdd(characterLabel, false);
                nexusParser->logMesssage(
                            QString("%1 BLOCK: created character label [C%2] -> %3")
                            .arg(blockID)
                            .arg(n)
                            .arg(characterLabel)
                            );
            } else {
                characterAdd(characterLabel, true);
                nexusParser->logMesssage(
                            QString("%1 BLOCK: created character label [C%2] [ELIMINATED] -> %3")
                            .arg(blockID)
                            .arg(n)
                            .arg(characterLabel)
                            );
            }
        }

        currChar++;

        Q_ASSERT(n == currChar);

        // Token should be the character label
        token.getNextToken();
        QString characterLabel = token.getToken();
        if (!isEliminated(currChar - 1)) {
            characterAdd(characterLabel, false);
            nexusParser->logMesssage(
                        QString("%1 BLOCK: extracted character label [C%2] -> %3")
                        .arg(blockID)
                        .arg(n)
                        .arg(characterLabel)
                        );
        } else {
            characterAdd(characterLabel, true);
            nexusParser->logMesssage(
                        QString("%1 BLOCK: extracted character label [C%2] [ELIMINATED] -> %3")
                        .arg(blockID)
                        .arg(n)
                        .arg(characterLabel)
                        );
        }
        // Token should be a slash character if state labels were provided for this character; otherwise,
        // token should be one of the following:
        // 1) the comma separating information for different characters, in which case we read in the
        //	  next token (which should be the next character number)
        // 2) the semicolon indicating the end of the command
        token.getNextToken();
        if (!token.equals("/")){
            if (!token.equals(",") && !token.equals(";")){
                errorMessage = "Expecting a comma or semicolon here, but found (";
                errorMessage += token.getToken();
                errorMessage += ") instead";
                throw NexusParserException(errorMessage,
                                   token.getFilePosition(),
                                   token.getFileLine(),
                                   token.getFileColumn());
            }

            // If ',' get next character number
            if (token.equals(",")){
                token.getNextToken();
            }

            // Miss next bit of code and start next loop item...
            tokenAlreadyRead = true;
            continue;
        }

        // Now create a new association for the character states list
        int s = 0;
        for (;;) // loop #2
        {
            token.getNextToken();

            // End of CHARSTATELABELS found therefore break loop.
            if (token.equals(";")){
                semicolonFoundInInnerLoop = true;
                break;
            }
            // Will be expecting a new character number after this; therefore break loop.
            if (token.equals(",")){
                break;
            }

            if (datatype == continuous){
                throw NexusParserException("State Labels cannot be specified when the datatype is continuous",
                                   token.getFilePosition(),
                                   token.getFileLine(),
                                   token.getFileColumn());
            }

            // Token should be a character state label; add it to the Character class held in characterList as new State.
            QString stateLabel = token.getToken();
            QString stateSymbol = QString(symbols.at(s));
            characterList[n].addState(stateSymbol, stateLabel, "");

            nexusParser->logMesssage(
                        QString("%1 BLOCK: extracted character [C%2] state label -> [%3] %4")
                        .arg(blockID)
                        .arg(n)
                        .arg(stateSymbol)
                        .arg(stateLabel)
                        );

            s++;
        } // end loop #2

    } // end loop #1

    newchar = false;
}

// Called when CHARLABELS command needs to be parsed from within the DIMENSIONS block. Deals with everything after
// the token CHARLABELS up to and including the semicolon that terminates the CHARLABELS command. If an ELIMINATE
// command has been processed, labels for eliminated characters will not be stored.
void NexusParserCharactersBlock::handleCharlabels(NexusParserToken &token)
{
    nexusParser->logMesssage(
                QString("%1 BLOCK: found command \"CHARLABELS\" on line %2...")
                .arg(blockID)
                .arg(token.getFileLine())
                );

    int n = 0;
    characterList.clear();

    // If there has been no ELIMINATE command build a new charPos map - will be a simple map where key === value.
    if (charPos.isEmpty()){
        buildCharPosArray();
    }

    for (;;)
    {
        token.getNextToken();

        // Token should either be ';' or the name of a character (an isolated '_' character is
        // converted automatically by token.getNextToken() into a space, which is then stored
        // as the character label)
        if (token.equals(";")){
            break;
        } else {
            n++;

            // Check to make sure user is not trying to read in more character labels than there are characters
            if (n > ncharTotal){
                throw NexusParserException("Number of character labels exceeds NCHAR specified in DIMENSIONS command",
                                   token.getFilePosition(),
                                   token.getFileLine(),
                                   token.getFileColumn());
            }

            QString characterLabel = token.getToken();
            if (!isEliminated(n - 1)) {
                characterAdd(characterLabel, false);
                nexusParser->logMesssage(
                            QString("%1 BLOCK: extracted character label [C%2] -> %3")
                            .arg(blockID)
                            .arg(n)
                            .arg(characterLabel)
                            );
            } else {
                characterAdd(characterLabel, true);
                nexusParser->logMesssage(
                            QString("%1 BLOCK: extracted character label [C%2] [ELIMINATED] -> %3")
                            .arg(blockID)
                            .arg(n)
                            .arg(characterLabel)
                            );
            }
        }
    }
    newchar = false;
}

// Called when STATELABELS command needs to be parsed from within the DIMENSIONS block. Deals with everything after
// the token STATELABELS up to and including the semicolon that terminates the STATELABELS command. Note that the
// numbers of states are shifted back one before being stored so that the character numbers in the NxsStringVectorMap
// objects are 0-offset rather than being 1-offset as in the NexusParserReader data file.
void NexusParserCharactersBlock::handleStatelabels(NexusParserToken &token)
{
    nexusParser->logMesssage(
                QString("%1 BLOCK: found command \"STATELABELS\" on line %2...")
                .arg(blockID)
                .arg(token.getFileLine())
                );

    bool semicolonFoundInInnerLoop = false;

    if (datatype == continuous){
        throw NexusParserException("STATELABELS cannot be specified when the datatype is continuous",
                           token.getFilePosition(),
                           token.getFileLine(),
                           token.getFileColumn());
    }

    if (characterList.count() != ncharTotal) {
        throw NexusParserException("STATELABELS cannot be specified before the CHARLABELS command",
                           token.getFilePosition(),
                           token.getFileLine(),
                           token.getFileColumn());
    }

    if (charPos.isEmpty()){
        buildCharPosArray();
    }

    for (;;)
    {
        if (semicolonFoundInInnerLoop){
            break;
        }

        token.getNextToken();

        if (token.equals(";")){
            break;
        }

        // Token should be the character number; create a new association
        int n = token.getToken().toInt();

        if (n < 1 || n > ncharTotal){
            errorMessage = "Invalid character number (";
            errorMessage += token.getToken();
            errorMessage += ") found in STATELABELS command (either out of range or not interpretable as an integer)";
            throw NexusParserException(errorMessage,
                               token.getFilePosition(),
                               token.getFileLine(),
                               token.getFileColumn());
        }

        int s = 0;
        for (;;)
        {
            token.getNextToken();

            if (token.equals(";")){
                semicolonFoundInInnerLoop = true;
                break;
            }

            if (token.equals(",")){
                break;
            }

            // Token should be a character state label; add it to the list           
            QString stateLabel = token.getToken();
            QString stateSymbol = QString(symbols.at(s));
            characterList[n-1].addState(stateSymbol, stateLabel, "");
            nexusParser->logMesssage(
                        QString("%1 BLOCK: extracted character [C%2] state label -> [%3] %4")
                        .arg(blockID)
                        .arg(n)
                        .arg(stateSymbol)
                        .arg(stateLabel)
                        );
            s++;
        }
    }
}

// Called when MATRIX command needs to be parsed from within the CHARACTERS block. Deals with everything after the
// token MATRIX up to and including the semicolon that terminates the MATRIX command.
void NexusParserCharactersBlock::handleMatrix(NexusParserToken &token)
{
    nexusParser->logMesssage(
                QString("%1 BLOCK: found command \"MATRIX\" on line %2...")
                .arg(blockID)
                .arg(token.getFileLine())
                );

    if (ntax == 0){
       errorMessage = "Must precede ";
       errorMessage += blockID;
       errorMessage += " block with a TAXA block or specify NEWTAXA and NTAX in the DIMENSIONS command";
       throw NexusParserException(errorMessage,
                          token.getFilePosition(),
                          token.getFileLine(),
                          token.getFileColumn());
    }

    if (ntaxTotal == 0) {
        ntaxTotal = taxaBlock->getNumTaxonLabels();
    }

    // Initialize the QLists activeTaxon and activeChar. All characters and all taxa are initially active.
    for (int i = 0; i < ntax; i++){
        activeTaxon.insert(i, true);
    }

    for (int i = 0; i < nchar; i++){
        activeChar.insert(i, true);
    }

    // The value of ncharTotal is normally identical to the value of nchar specified
    // in the CHARACTERS block DIMENSIONS command.	If an ELIMINATE command is
    // processed, however, nchar < ncharTotal.	Note that the ELIMINATE command
    // will have already been read by now, and the eliminated character numbers
    // will be stored in the QMap<int, int> eliminated.
    // Note that if an ELIMINATE command has been read, charPos will have already
    // been created; thus, we only need to allocate and initialize charPos if user
    // did not specify an ELIMINATE command
    if (charPos.isEmpty()){
        buildCharPosArray();
    }

    // The value of ntaxTotal equals the number of taxa specified in the
    // TAXA block, whereas ntax equals the number of taxa specified in
    // the DIMENSIONS command of the CHARACTERS block.	These two numbers
    // will be identical unless some taxa were left out of the MATRIX
    // command of the CHARACTERS block, in which case ntax < ntaxTotal.
    if (!taxonPos.isEmpty()){
        taxonPos.clear();
    }

    for (int i = 0; i < ntaxTotal; i++){
        taxonPos.insert(i, INT_MAX);
    }

    if (datatype == NexusParserCharactersBlock::continuous){
        nexusParser->logMesssage(
                    QString("%1 BLOCK: MATRIX: is made of CONTINUOUS data.")
                    .arg(blockID)
                    );
    } else {
        nexusParser->logMesssage(
                    QString("%1 BLOCK: MATRIX: is made of DISCRETE data.")
                    .arg(blockID)
                    );
    }

    if (transposing){
        nexusParser->logMesssage(
                    QString("%1 BLOCK: MATRIX: is TRANSPOSED.")
                    .arg(blockID)
                    );
        handleTransposedMatrix(token);
    } else {
        nexusParser->logMesssage(
                    QString("%1 BLOCK: MATRIX: is STANDARD.")
                    .arg(blockID)
                    );
        handleStandardMatrix(token);
    }

    demandEndSemicolon(token, "MATRIX");

    // If we've gotten this far, presumably it is safe to
    // tell the ASSUMPTIONS block that were ready to take on
    // the responsibility of being the current character-containing
    // block (to be consulted if characters are excluded or included
    // or if taxa are deleted or restored)
    //assumptionsBlock->setCallback(this);
}

// Called from HandleMatrix function to read in a standard (i.e., non-transposed) matrix. Interleaving, if
// applicable, is dealt with herein.
void NexusParserCharactersBlock::handleStandardMatrix(NexusParserToken &token)
{
    Q_ASSERT(!taxonPos.isEmpty());
    Q_ASSERT(!taxonPos.isEmpty());

    nexusParser->logMesssage(
                QString("%1 BLOCK: MATRIX: running standard matrix reader...")
                .arg(blockID)
                );

    int page = 0;               // current page
    int currentCharacter = 0;   // current character
    int currentTaxon = 0;       // current taxon
    int firstChararcter = 0;    // first character
    int lastChararcter = ncharTotal;     // last character
    int nextFirst = 0;

    for (;;) // loop #1
    {
        // Beginning of loop through taxa
        for (currentTaxon = 0; currentTaxon < ntax; currentTaxon++) // loop #2
        {
            if(labels) {
                // This should be the taxon label
                token.getNextToken();

                if (page == 0 && newtaxa) {
                    // This section:
                    // - labels provided
                    // - on first (or only) interleave page
                    // - no previous TAXA block

                    // Check for duplicate taxon names
                    if (taxaBlock->taxonIsDefined(token.getToken())) {
                        errorMessage = "Data for this taxon (";
                        errorMessage += token.getToken();
                        errorMessage += ") has already been saved";
                        throw NexusParserException(errorMessage,
                                           token.getFilePosition(),
                                           token.getFileLine(),
                                           token.getFileColumn());
                    }

                    // Labels provided and not already stored in the taxa block with the TAXLABELS command; taxaBlock->reset()
                    // and taxaBlock->setNTax() have were already called, however, when the NTAX subcommand was processed.
                    // Order of occurrence in TAXA block same as row in matrix
                    QString currentToken = token.getToken();
                    int positionInTaxaBlockList = taxaBlock->taxonAdd(currentToken);

                    nexusParser->logMesssage(
                                QString("%1 BLOCK: MATRIX: found taxon [T%2] \"%3\". Attempting to extract chararcter states...")
                                .arg(blockID)
                                .arg(positionInTaxaBlockList+1)
                                .arg(currentToken)
                                );

                    taxonPos.insert(positionInTaxaBlockList, currentTaxon);
                } else {
                    // This section:
                    // - labels provided
                    // - TAXA block provided or has been created already
                    // - may be on any (interleave) page

                    // Cannot assume taxon in same position in taxa block. Will need to look up current positions and move if needed.
                    int positionInTaxaBlockList;
                    QString currentToken = token.getToken();

                    try {
                        positionInTaxaBlockList = taxaBlock->taxonFind(currentToken);
                    } catch (NexusParserTaxaBlock::NexusParserX_NoSuchTaxon) {
                        if (token.equals(";") && currentTaxon == 0) {
                            errorMessage = "Unexpected ; (after only ";
                            errorMessage += currentCharacter;
                            errorMessage += " characters were read)";
                        } else {
                            errorMessage = "Could not find taxon named ";
                            errorMessage += currentToken;
                            errorMessage += " among stored taxon labels";
                        }
                        throw NexusParserException(errorMessage,
                                           token.getFilePosition(),
                                           token.getFileLine(),
                                           token.getFileColumn());
                    }

                    nexusParser->logMesssage(
                                QString("%1 BLOCK: MATRIX: found taxon [T%2] \"%3\". Attempting to extract chararcter states...")
                                .arg(blockID)
                                .arg(positionInTaxaBlockList+1)
                                .arg(currentToken)
                                );

                    if (page == 0) {
                        // Make sure user has not duplicated data for a single taxon
                        if (taxonPos[positionInTaxaBlockList] != INT_MAX) {
                            errorMessage = "Data for this taxon (";
                            errorMessage += token.getToken();
                            errorMessage += ") has already been saved";
                            throw NexusParserException(errorMessage,
                                               token.getFilePosition(),
                                               token.getFileLine(),
                                               token.getFileColumn());
                        }


                        // Make sure user has kept same relative ordering of taxa in both the TAXA block and the CHARACTERS block
                        if (positionInTaxaBlockList != currentTaxon) {
                            throw NexusParserException("Relative order of taxa must be the same in both the TAXA and CHARACTERS blocks",
                                               token.getFilePosition(),
                                               token.getFileLine(),
                                               token.getFileColumn());
                        }
                        taxonPos.insert(currentTaxon, positionInTaxaBlockList);
                    } else {
                        // Make sure user has kept the ordering of taxa the same from one interleave page to the next
                        if (taxonPos[positionInTaxaBlockList] != currentTaxon) {
                            throw NexusParserException("Ordering of taxa must be identical to that in first interleave page",
                                               token.getFilePosition(),
                                               token.getFileLine(),
                                               token.getFileColumn());
                        }
                    }
                }
            } else {
                // No labels provided, assume taxon position same as in taxa block
                nexusParser->logMesssage(
                            QString("%1 BLOCK: MATRIX: found taxon [T%2] [NO LABEL]. Attempting to extract chararcter states...")
                            .arg(blockID)
                            .arg(currentTaxon+1)
                            );

                if (page == 0){
                    taxonPos.insert(currentTaxon,currentTaxon);
                }
            }

            // Begin loop through characters
            for (currentCharacter = firstChararcter; currentCharacter < lastChararcter; currentCharacter++) // loop #3
            {
                // As we have stored all characters found in theNEXUS file we expect there to be the same number of
                // character columns too. We do not eliminate any here as that information is already stored in the
                // Character data object with the characterList. Therefore can exclude the data as needs be later on.

                // ok will be false only if a newline character is encountered before character j processed
                bool ok = handleNextState(token, currentTaxon, currentCharacter);

                if (interleaving && !ok){
                    if (lastChararcter < ncharTotal && currentCharacter != lastChararcter) {
                        throw NexusParserException("Each line within an interleave page must comprise the same number of characters",
                                           token.getFilePosition(),
                                           token.getFileLine(),
                                           token.getFileColumn());
                    }

                    // currentCharacter should be firstChar in next go around
                    nextFirst = currentCharacter;

                    // Set lastChararcter to currentCharacter so that we can check to make sure the remaining lines in this interleave
                    // page end at the same place
                    lastChararcter = currentCharacter;
                }

            } // end loop #3

        } // end loop #2

        firstChararcter = nextFirst;
        lastChararcter = ncharTotal;

        // If currentCharacter equals ncharTotal, then we've just finished reading the last interleave page
        // and thus should break from the outer loop. Note that if we are not interleaving, this will
        // still work since lastChar is initialized to ncharTotal and never changed
        if (currentCharacter == ncharTotal) {
            break;
        }

        page++;
    } // end loop #1
}

void NexusParserCharactersBlock::handleTransposedMatrix(NexusParserToken &token)
{
    nexusParser->logMesssage(
                QString("%1 BLOCK: MATRIX: running tranposed matrix reader...")
                .arg(blockID)
                );
}

// Called from handleStandarMatrix or handleTransposedMatrix function to read in the next state. Always returns true
// except in the special case of an interleaved matrix, in which case it returns false if a newline character is
// encountered before the next token.

bool NexusParserCharactersBlock::handleNextState(NexusParserToken &token, int currentTaxon, int currentCharacter)
{
    int taxonID = taxaBlock->getTaxonID(currentTaxon);
    int characterID = characterList[currentCharacter].getID();

    if (interleaving) {
        token.setLabileFlagBit(NexusParserToken::newlineIsToken);
    }

    if (datatype == NexusParserCharactersBlock::continuous){
        // TO DO .... as the application may want to allow the use of contiuous data.
    }

    // This should be the state for taxon 'currentTaxon' and character 'currentCharacter'
    if (!tokens){
        token.setLabileFlagBit(NexusParserToken::parentheticalToken);
        token.setLabileFlagBit(NexusParserToken::curlyBracketedToken);
        token.setLabileFlagBit(NexusParserToken::singleCharacterToken);
    }

    token.getNextToken();

    if (interleaving && token.getAtEndOfLine()){
        return false;
    }

    // Make sure we didn't run out of file
    if (token.getAtEndOfFile()) {
        throw NexusParserException("Unexpected end of file encountered while reading the MATRIX BLOCK",
                           token.getFilePosition(),
                           token.getFileLine(),
                           token.getFileColumn());
    }

    // If we didn't run out of file, there is no reason why we should have a zero-length token on our hands
    Q_ASSERT(token.getTokenLength() > 0);

    // See if any equate macros apply. Equates should always respect case.
    QString symbol = token.getToken(true);
    for(int i = 0; i < equatesList.count(); i++)
    {
        if (equatesList[i].getSymbol() == symbol) {
            token.replaceToken(equatesList[i].getEquivalent());
        }
    }

    // Handle case of single-character state symbol
    if (!tokens && token.getTokenLength() == 1) {

        QChar ch = token.getToken()[0];

        // Check for missing data symbol and add to martixGrid if found
        if (ch == missing) {
            cellAdd(taxonID, characterID, QString(missing), "", false, true, false, false, false);
        }
        // Check for matchchar symbol and add to martixGrid if found
        else if (ch == matchchar) {
            cellAdd(taxonID, characterID, QString(matchchar), "", false, false, false, true, false);
        }
        // Check for gap symbol and add to martixGrid if found
        else if (ch == gap) {
            cellAdd(taxonID, characterID, QString(gap), "", false, false, true, false, false);
        }
        // Look up the position of this state in the symbols array
        else {
            // Check the symbol is found in the symbols list
            if(!isInSymbols(ch)){
                errorMessage = "State specified (";
                errorMessage += token.getToken();
                errorMessage += ") for taxon ";
                errorMessage += (currentTaxon+1);
                errorMessage += ", character ";
                errorMessage += (currentCharacter+1);
                errorMessage += ", not found in list of valid symbols";
                throw NexusParserException(errorMessage,
                                   token.getFilePosition(),
                                   token.getFileLine(),
                                   token.getFileColumn());
            }
            cellAdd(taxonID, characterID, QString(ch), "", false, false, false, false, false);
        }
    }
    // Handle case of state sets when tokens is not in effect
    else if (!tokens && token.getTokenLength() > 1) {
        // Token should be in one of the following forms: {acg} {a~g} {a c g} (acg) (a~g) (a c g)
        QString t = token.getToken();
        int len = t.size();
        bool polymorphic = (t[0] == QChar('('));
        bool uncertainty = (t[0] == QChar('{'));
        bool tildeFound = false;

        Q_ASSERT(polymorphic || uncertainty);
        Q_ASSERT((polymorphic && t[len-1] == QChar(')')) || (uncertainty && t[len-1] == QChar('}')));

        int firstNonBlank = 1;
        while (t[firstNonBlank] == QChar(' ') || t[firstNonBlank] == QChar('\t')) {
            firstNonBlank++;
        }

        int lastNonBlank = len-2;
        while (t[lastNonBlank] == QChar(' ') || t[lastNonBlank] == QChar('\t')) {
            lastNonBlank--;
        }

        if (t[firstNonBlank] == QChar('~') || t[lastNonBlank] == QChar('~')) {
            errorMessage = token.getToken();
            errorMessage += " does not represent a valid range of states";
            throw NexusParserException(errorMessage,
                               token.getFilePosition(),
                               token.getFileLine(),
                               token.getFileColumn());
        }

        int i = 1;
        QChar lastRead;
        QString newToken = QString(t[0]);
        for(;;) // loop #1
        {
            if (t[i] == QChar(')') || t[i] == QChar('}')) {
                break;
            }

            if (t[i] == QChar(' ') || t[i] == QChar('\t') || t[i] == QChar(',')) {
                i++;
                continue;
            }

            // t[i] should be either '~' or one of the state symbols
            if (t[i] == QChar('~')){
                tildeFound = true;
            } else {
                // Add state symbol and record if it is the first or last one in case we encounter a tilde
                if (tildeFound){
                    // Go back to last entered symbol and find position in symbolsList, then add every symbol
                    // up until and including the current one.
                    int startPosition = symbols.indexOf(lastRead)+1;
                    newToken += QString(t[startPosition]);
                    for(int s = startPosition; t[i] != symbols[s]; s++){
                        newToken += QString(t[s]);
                    }
                    newToken += QString(t[i]);
                    tildeFound = false;
                } else {
                    // Check all states are valid
                    if(!isInSymbols(t[i])){
                        errorMessage = "State specified (";
                        errorMessage += t[i];
                        errorMessage += ") for taxon ";
                        errorMessage += (currentTaxon+1);
                        errorMessage += ", character ";
                        errorMessage += (currentCharacter+1);
                        errorMessage += ", not found in list of valid symbols";
                        throw NexusParserException(errorMessage,
                                           token.getFilePosition(),
                                           token.getFileLine(),
                                           token.getFileColumn());

                    }
                    lastRead = t[i];
                    newToken += QString(t[i]);
                }
            }
            i++;
        } // end loop #1
        newToken += QString(t[len-1]);
        cellAdd(taxonID, characterID, newToken, "", polymorphic, false, false, false, uncertainty);
    }
    // Handle case in which TOKENS was specified in the FORMAT command
    else {
        // TO DO... but not for a while as application wont deal with token just yet!
    }

    return true;
}

/*------------------------------------------------------------------------------------/
 * Other Functions
 *-----------------------------------------------------------------------------------*/

// Returns true if `ch' can be found in the `symbols' list. The value of `respectingCase' is used to determine
// whether or not the search should be case sensitive. Assumes `symbols' is non-NULL.
bool NexusParserCharactersBlock::isInSymbols(QChar ch)
{
    ch = respectingCase ?  ch : ch.toUpper();
    bool found = false;
    for (int i = 0; i < symbols.count(); i++)
    {
        QChar charInSymbols = respectingCase ?  symbols.at(i) : symbols.at(i).toUpper();

        if (charInSymbols != ch) {
            continue;
        }

        found = true;
        break;
    }

    return found;
}

// Resets standard symbol set after a change in `datatype' is made. Also flushes equates list and installs standard
// equate macros for the current `datatype'.
void NexusParserCharactersBlock::resetSymbols()
{
    symbols.clear();

    switch(datatype)
    {
        case NexusParserCharactersBlock::dna:
            // "ACGT"
            for(int i = 0; i < nexusParser->defaultDNAStateSet.count(); i++)
            {
                symbols.append(nexusParser->defaultDNAStateSet.at(i).at(0));
            }
            break;

        case NexusParserCharactersBlock::rna:
            // "ACGU";
            for(int i = 0; i < nexusParser->defaultRNAStateSet.count(); i++)
            {
                symbols.append(nexusParser->defaultRNAStateSet.at(i).at(0));
            }
            break;

        case NexusParserCharactersBlock::nucleotide:
            // "ACGT"
            for(int i = 0; i < nexusParser->defaultNucleotideStateSet.count(); i++)
            {
                symbols.append(nexusParser->defaultNucleotideStateSet.at(i).at(0));
            }
            break;

        case NexusParserCharactersBlock::protein:
            // "ACDEFGHIKLMNPQRSTVWY*"
            for(int i = 0; i < nexusParser->defaultProteinStateSet.count(); i++)
            {
                symbols.append(nexusParser->defaultProteinStateSet.at(i).at(0));
            }
            break;

        default:
            // "01"
            for(int i = 0; i < nexusParser->defaultStandardStateSet.count(); i++)
            {
                symbols.append(nexusParser->defaultStandardStateSet.at(i).at(0));
            }
            break;
    }

    equatesList.clear();
    setDefaultEquates();
}

// Converts a taxon label to a number corresponding to the taxon's position within the list maintained by the
// NexusParserTaxaBlock object. This method overrides the virtual function of the same name in the NexusParserBlock base class. If
// `str' is not a valid taxon label, returns the value 0.
int NexusParserCharactersBlock::taxonLabelToNumber(QString str)
{
    int i;
    try {
        i = 1 + taxaBlock->taxonFind(str);
    } catch(NexusParserTaxaBlock::NexusParserX_NoSuchTaxon){
        i = 0;
    }

    return i;
}

// Converts a character label to a 1-offset number corresponding to the character's position within `charLabels'. This
// method overrides the virtual function of the same name in the NexusParserBlock base class. If `str' is not a valid character
// label, returns the value 0.
int NexusParserCharactersBlock::charLabelToNumber(QString str)
{
    int k = 0;
    for (int i = 0; i < characterList.count(); ++i)
    {
        if (characterList[i].characterName == str){
            return i+1;
        }
    }

    return k;
}

// Returns true if character number `origCharIndex' was eliminated, false otherwise. Returns false immediately if
// `eliminated' set is empty.
bool NexusParserCharactersBlock::isEliminated(int origCharIndex)
{
    // Note: it is tempting to try to streamline this method by just looking up character j in charPos to see if it
    // has been eliminated, but this temptation should be resisted because this function is used in setting up
    // charPos in the first place!

    if (eliminated.empty()){
        return false;
    }

    for(int i = 0; i < eliminated.count(); i++)
    {
        if (eliminated.contains(origCharIndex)){
            return true;
        }
    }
    return false;
}

/*------------------------------------------------------------------------------------/
 * Character Data Functions
 *-----------------------------------------------------------------------------------*/

// Add a new Character data object to the characterList
int NexusParserCharactersBlock::characterAdd(QString characterLabel, bool isEliminated) {
    isEmpty = false;
    Character newCharacter = Character(nextCharacterID,characterLabel,"");
    newCharacter.setIsEliminated(isEliminated);
    characterList.append(newCharacter);
    nextCharacterID++;
    return (characterList.count());
}

// Edit character label in list at characterList position
void NexusParserCharactersBlock::characterLabelEdit(int position, QString characterLabel) {
    characterList[position].setLabel(characterLabel);
}

// Returns current index of character in matrix. This may differ from the original index if some characters were
// removed using an ELIMINATE command. For example, character number 9 in the original data matrix may now be at
// position 8 if the original character 8 was eliminated. The parameter `origCharIndex' is assumed to range from
// 0 to `ncharTotal' - 1.
int NexusParserCharactersBlock::getCharPos(int origCharIndex)
{
    Q_ASSERT(!charPos.isEmpty());
    Q_ASSERT(origCharIndex < ncharTotal);

    return charPos.value(origCharIndex);
}

// Use to initialize `charPos' map, which keeps track of the original character index in
// cases where characters have been eliminated. This function is called by handleEliminate in response to encountering
// an ELIMINATE command in the data file, and this is probably the only place where buildCharPosArray should be called
// with `checkEliminated' true. buildCharPosArray is also called in handleMatrix, handleCharstatelabels,
// handleStatelabels, and handleCharlabels.
void NexusParserCharactersBlock::buildCharPosArray(bool checkEliminated)
{
    charPos.clear();

    int k = 0;
    for (int j = 0; j < ncharTotal; j++)
    {
        if (checkEliminated && isEliminated(j)){
            charPos.insert(j, INT_MAX);
        } else {
            charPos.insert(j, k++);
        }
    }
}

/*------------------------------------------------------------------------------------/
 * Equate Data Functions
 *-----------------------------------------------------------------------------------*/

// Add a new Equate data object to the equatesList
int NexusParserCharactersBlock::equateAdd(
        QString symbol,
        QString equivalent
        )
{
    equatesList.append(Equate(nextEquatesID,symbol, equivalent));
    nextEquatesID++;
    return (equatesList.count());
}

QList<Equate> NexusParserCharactersBlock::setDefaultEquates()
{
    QStringList equatesStringList;

    switch(datatype)
    {
        case NexusParserCharactersBlock::dna:
            equatesStringList = nexusParser->defaultDNAEquateStates;
            break;

        case NexusParserCharactersBlock::rna:
            equatesStringList = nexusParser->defaultRNAEquateStates;
            break;

        case NexusParserCharactersBlock::nucleotide:
            equatesStringList = nexusParser->defaultNucleotideEquateStates;
            break;

        case NexusParserCharactersBlock::protein:
            equatesStringList = nexusParser->defaultProteinEquateStates;
            break;

        default:
            return equatesList;
            break;
    }

    // Create equatesMap from equatesList;
    if (equatesStringList.count() > 0) {
        for(int i = 0; i < equatesStringList.count(); i++)
        {
            QRegExp rx("(=)");
            QStringList pair = equatesStringList.at(i)
                    .trimmed()
                    .replace(" ","")
                    .split(rx);
            equateAdd(pair[0], pair[1]);
        }
    }
    // Create lowercase keys of the stored uppercase keys/value pairs
    int count = equatesList.count();
    for(int i = 0; i < count; i++) {
        QString uppercase, lowercase;
        uppercase = equatesList[i].getSymbol();
        lowercase = uppercase.toLower();
        equateAdd(lowercase, equatesList[i].getEquivalent());
    }

    return equatesList;
}

/*------------------------------------------------------------------------------------/
 * Matrix Grid Data Functions
 *-----------------------------------------------------------------------------------*/

// Add Data Cell
bool NexusParserCharactersBlock::cellAdd(
        int taxonID,
        int characterID,
        QString state,
        QString notes,
        bool isPolymorphic,
        bool isMissing,
        bool isGap,
        bool isMatchchar,
        bool isUncertain
        )
{
    Cell *cellData = new Cell(state, notes);

    if (isPolymorphic) {
        cellData->setPolymorphic(true);
    } else if (isMissing) {
        cellData->setMissing(true);
    } else if (isGap) {
        cellData->setGap(true);
    } else if (isMatchchar) {
        cellData->setMatchchar(true);
    } else if (isUncertain) {
        cellData->setUncertainty(true);
    }

    matrixGrid.insert(returnLocator(taxonID, characterID), cellData);

    return true;
}

// Cell count
int NexusParserCharactersBlock::cellCount()
{
    return matrixGrid.count();
}

// Create Cell Locator
QPair<int,int> NexusParserCharactersBlock::returnLocator(
        int taxonID,
        int characterID
        )
{
    QPair<int,int> locator;
    locator.first = taxonID;
    locator.second = characterID;
    return locator;
}
