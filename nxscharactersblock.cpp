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

NxsCharactersBlock::NxsCharactersBlock(NxsReader *pointer, NxsTaxaBlock *tBlock)
{
    Q_ASSERT(tBlock != NULL);
    //assert(aBlock != NULL);

    setNxsReader(pointer);

    blockID = "CHARACTERS";

    taxaBlock = tBlock;

    reset();
}

NxsCharactersBlock::~NxsCharactersBlock()
{
    reset();

    if(symbols.count() > 0) {
        symbols.clear();
    }
}

/*------------------------------------------------------------------------------------/
 * Read Function
 *-----------------------------------------------------------------------------------*/

// This function provides the ability to read everything following the block name (which is read by the NxsReader
// object) to the END or ENDBLOCK statement. Characters are read from the input stream `in'. Overrides the abstract
// virtual function in the base class.
void NxsCharactersBlock::read(NxsToken &token)
{
    isEmpty = false;
    demandEndSemicolon(token, QString("BEGIN %1").arg(blockID));

    // Get Taxa Number (nTax)
    ntax = taxaBlock->getNumTaxonLabels();
    for(;;)
    {
        token.getNextToken();
        NxsBlock::NxsCommandResult result = handleBasicBlockCommands(token);
        if (result == NxsBlock::NxsCommandResult(STOP_PARSING_BLOCK)){
            return;
        }
        if (result != NxsBlock::NxsCommandResult(HANDLED_COMMAND)) {
            if (token.equals("DIMENSIONS")) {
                handleDimensions(token, "NEWTAXA", "NTAX", "NCHAR");
            } else if (token.equals("FORMAT")) {
                handleFormat(token);
            } else if (token.equals("ELIMINATE")) {
                handleEliminate(token);
            } else if (token.equals("TAXLABELS")) {
                handleTaxlabels(token);
            } else if (token.equals("CHARSTATELABELS")) {
                //handleCharstatelabels(token);
            } else if (token.equals("CHARLABELS")) {
                //handleCharlabels(token);
            } else if (token.equals("STATELABELS")) {
                //handleStatelabels(token);
            } else if (token.equals("MATRIX")) {
                //handleMatrix(token);
            } else {
                skippingCommand(token.getToken());
                do {
                    token.getNextToken();
                } while (!token.getAtEndOfFile() && !token.equals(";"));

                if (token.getAtEndOfFile()){
                    errorMessage = "Unexpected end of file encountered";
                    throw NxsException(errorMessage, token.getFilePosition(), token.getFileLine(), token.getFileColumn());
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
void NxsCharactersBlock::handleDimensions(NxsToken &token, QString newtaxaLabel, QString ntaxLabel, QString ncharLabel)
{
    nxs->NxsLogMesssage(
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

            nxs->NxsLogMesssage(
                        QString("%1 BLOCK: found subcommand \"%2\" on line %3.")
                        .arg(blockID)
                        .arg(newtaxaLabel)
                        .arg(token.getFileLine())
                        );
        } else if (token.equals(ntaxLabel)){
            demandEquals(token, "in DIMENSIONS command");
            ntax = demandPositiveInt(token, ntaxLabel);

            nxs->NxsLogMesssage(
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
                    throw NxsException(errorMessage, token.getFilePosition(), token.getFileLine(), token.getFileColumn());
                }
            }
        } else if (token.equals(ncharLabel)){
            demandEquals(token, "in DIMENSIONS command");
            nchar = demandPositiveInt(token, ncharLabel);
            ncharTotal = nchar;

            nxs->NxsLogMesssage(
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
void NxsCharactersBlock::handleFormat(NxsToken &token)
{
    nxs->NxsLogMesssage(
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
                throw NxsException(errorMessage, token.getFilePosition(), token.getFileLine(), token.getFileColumn());
            }

            nxs->NxsLogMesssage(
                        QString("%1 BLOCK: found subcommand \"DATATYPE\" on line %2. DATATYPE = %3.")
                        .arg(blockID)
                        .arg(token.getFileLine())
                        .arg(token.getToken())
                        );

            if (standardDataTypeAssumed && datatype != standard) {
                throw NxsException("DATATYPE must be specified first in FORMAT command", token.getFilePosition(), token.getFileLine(), token.getFileColumn());
            }

            resetSymbols();

            if (datatype == continuous) {
                tokens = true;
            }

        } else if (token.equals("RESPECTCASE")) {
            if (ignoreCaseAssumed) {
                throw NxsException("RESPECTCASE must be specified before MISSING, GAP, SYMBOLS, and MATCHCHAR in FORMAT command", token.getFilePosition(), token.getFileLine(), token.getFileColumn());
            }

            nxs->NxsLogMesssage(
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
                throw NxsException(errorMessage, token.getFilePosition(), token.getFileLine(), token.getFileColumn());
            } else if (token.isPunctuationToken() && !token.isPlusMinusToken()){
                errorMessage = "MISSING symbol specified cannot be a punctuation token (";
                errorMessage += token.getToken();
                errorMessage += " was specified)";
                throw NxsException(errorMessage, token.getFilePosition(), token.getFileLine(), token.getFileColumn());
            } else if (token.isWhitespaceToken()) {
                errorMessage = "MISSING symbol specified cannot be a whitespace character (";
                errorMessage += token.getToken();
                errorMessage += " was specified)";
                throw NxsException(errorMessage, token.getFilePosition(), token.getFileLine(), token.getFileColumn());
            }

            missing = token.getToken().at(0);

            nxs->NxsLogMesssage(
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
                throw NxsException(errorMessage, token.getFilePosition(), token.getFileLine(), token.getFileColumn());
            } else if (token.isPunctuationToken() && !token.isPlusMinusToken()) {
                errorMessage = "GAP symbol specified cannot be a punctuation token (";
                errorMessage += token.getToken();
                errorMessage += " was specified)";
                throw NxsException(errorMessage, token.getFilePosition(), token.getFileLine(), token.getFileColumn());
            }  else if (token.isWhitespaceToken()) {
                errorMessage = "GAP symbol specified cannot be a whitespace character (";
                errorMessage += token.getToken();
                errorMessage += " was specified)";
                throw NxsException(errorMessage, token.getFilePosition(), token.getFileLine(), token.getFileColumn());
            }

            gap = token.getToken().at(0);

            nxs->NxsLogMesssage(
                        QString("%1 BLOCK: found subcommand \"GAP\" on line %2. GAP = %3.")
                        .arg(blockID)
                        .arg(token.getFileLine())
                        .arg(gap)
                        );

            ignoreCaseAssumed = true;
            standardDataTypeAssumed = true;
        } else if (token.equals("SYMBOLS")) {
            if (datatype == NxsCharactersBlock::continuous) {
                throw NxsException("SYMBOLS subcommand not allowed for DATATYPE=CONTINUOUS", token.getFilePosition(), token.getFileLine(), token.getFileColumn());
            }

            int numDefStates;
            int maxNewStates;
            switch(datatype){
                case NxsCharactersBlock::dna:
                case NxsCharactersBlock::rna:
                case NxsCharactersBlock::nucleotide:
                    numDefStates = 4;
                    maxNewStates = NXS_MAX_STATES-numDefStates;
                    break;

                case NxsCharactersBlock::protein:
                    numDefStates = 21;
                    maxNewStates = NXS_MAX_STATES-numDefStates;
                    break;

                default:
                    numDefStates = 0;
                    symbols.clear();
                    maxNewStates = NXS_MAX_STATES-numDefStates;
                    break;
            }

            demandEquals(token, " after keyword SYMBOLS");

            // This should be the symbols list
            token.setLabileFlagBit(NxsToken::doubleQuotedToken);
            token.getNextToken();
            token.stripWhitespace();

            int numNewSymbols = token.getTokenLength();

            if (numNewSymbols > maxNewStates){
                errorMessage = "SYMBOLS defines ";
                errorMessage += numNewSymbols;
                errorMessage += " new states but only ";
                errorMessage += maxNewStates;
                errorMessage += " new states allowed for this DATATYPE";
                throw NxsException(errorMessage, token.getFilePosition(), token.getFileLine(), token.getFileColumn());
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
                    throw NxsException(errorMessage, token.getFilePosition(), token.getFileLine(), token.getFileColumn());
                }
            }

            // If we've made it this far, go ahead and add the user-defined symbols to the end of the list of predefined symbols
            QString symbolsString;
            for (int i = 0; i < tlen; i++)
            {
                symbols.append(t.at(i));
                symbolsString.append(t.at(i));
            }

            nxs->NxsLogMesssage(
                        QString("%1 BLOCK: found subcommand \"SYMBOLS\" on line %2. SYMBOLS = %3.")
                        .arg(blockID)
                        .arg(token.getFileLine())
                        .arg(symbolsString)
                        );

            ignoreCaseAssumed = true;
            standardDataTypeAssumed = true;
        } else if (token.equals("EQUATE")) {

            if (datatype == NxsCharactersBlock::continuous) {
                throw NxsException("EQUATE subcommand not allowed for DATATYPE=CONTINUOUS", token.getFilePosition(), token.getFileLine(), token.getFileColumn());
            }

            demandEquals(token, "after keyword EQUATE");

            // This should be a double-quote character
            token.getNextToken();

            if (!token.equals("\"")) {
                errorMessage = "Expecting '\"' after keyword EQUATE but found ";
                errorMessage += token.getToken();
                errorMessage += " instead";
                throw NxsException(errorMessage, token.getFilePosition(), token.getFileLine(), token.getFileColumn());
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
                    throw NxsException(errorMessage, token.getFilePosition(), token.getFileLine(), token.getFileColumn());
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
                    throw NxsException(errorMessage, token.getFilePosition(), token.getFileLine(), token.getFileColumn());
                }

                QString t1 = token.getToken();
                demandEquals(token, " in EQUATE definition");

                // This should be the token to be substituted in for the equate symbol
                token.setLabileFlagBit(NxsToken::parentheticalToken);
                token.setLabileFlagBit(NxsToken::curlyBracketedToken);
                token.getNextToken();
                QString t2 = token.getToken();

                // Add the new equate association to the equates list
                equates[t1] = t2;
            }

            nxs->NxsLogMesssage(
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
                throw NxsException(errorMessage, token.getFilePosition(), token.getFileLine(), token.getFileColumn());
            } else if (token.isPunctuationToken() && !token.isPlusMinusToken()){
                errorMessage = "MATCHCHAR symbol specified cannot be a punctuation token (";
                errorMessage += token.getToken();
                errorMessage += " was specified) ";
                throw NxsException(errorMessage, token.getFilePosition(), token.getFileLine(), token.getFileColumn());
            } else if (token.isWhitespaceToken()){
                errorMessage = "MATCHCHAR symbol specified cannot be a whitespace character (";
                errorMessage += token.getToken();
                errorMessage += " was specified)";
                throw NxsException(errorMessage, token.getFilePosition(), token.getFileLine(), token.getFileColumn());
            }

            matchchar = token.getToken().at(0);

            nxs->NxsLogMesssage(
                        QString("%1 BLOCK: found subcommand \"MATCHCHAR\" on line %2. MATCHCHAR = %3.")
                        .arg(blockID)
                        .arg(token.getFileLine())
                        .arg(matchchar)
                        );

            ignoreCaseAssumed = true;
            standardDataTypeAssumed = true;
        } else if (token.equals("LABELS")) {
            labels = true;

            nxs->NxsLogMesssage(
                        QString("%1 BLOCK: found subcommand \"LABELS\" on line %2.")
                        .arg(blockID)
                        .arg(token.getFileLine())
                        );

            standardDataTypeAssumed = true;
        } else if (token.equals("NOLABELS")) {
            labels = false;

            nxs->NxsLogMesssage(
                        QString("%1 BLOCK: found subcommand \"NOLABELS\" on line %2.")
                        .arg(blockID)
                        .arg(token.getFileLine())
                        );

            standardDataTypeAssumed = true;
        } else if (token.equals("TRANSPOSE")) {
            transposing = true;

            nxs->NxsLogMesssage(
                        QString("%1 BLOCK: found subcommand \"TRANSPOSE\" on line %2.")
                        .arg(blockID)
                        .arg(token.getFileLine())
                        );

            standardDataTypeAssumed = true;
        } else if (token.equals("INTERLEAVE")) {
            interleaving = true;

            nxs->NxsLogMesssage(
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
            if (datatype == NxsCharactersBlock::continuous){
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
                    throw NxsException("Sorry, only ITEMS=STATES is supported for discrete datatypes at this time", token.getFilePosition(), token.getFileLine(), token.getFileColumn());
                }
                items.clear();
                items.append("STATES");
            }

            nxs->NxsLogMesssage(
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
                if (datatype == NxsCharactersBlock::continuous) {
                    if (token.equals("INDIVIDUALS")) {
                        statesFormat = INDIVIDUALS;
                    } else {
                        throw NxsException("Sorry, only STATESFORMAT=STATESPRESENT or STATESFORMAT=INDIVIDUALS are supported for continuous datatypes at this time", token.getFilePosition(), token.getFileLine(), token.getFileColumn());
                    }
                } else {
                    throw NxsException("Sorry, only STATESFORMAT=STATESPRESENT supported for discrete datatypes at this time", token.getFilePosition(), token.getFileLine(), token.getFileColumn());
                }
            }

            nxs->NxsLogMesssage(
                        QString("%1 BLOCK: found subcommand \"STATESFORMAT\" on line %2.")
                        .arg(blockID)
                        .arg(token.getFileLine())
                        );

            standardDataTypeAssumed = true;
        } else if (token.equals("TOKENS")) {
            tokens = true;

            nxs->NxsLogMesssage(
                        QString("%1 BLOCK: found subcommand \"TOKENS\" on line %2.")
                        .arg(blockID)
                        .arg(token.getFileLine())
                        );

            standardDataTypeAssumed = true;
        } else if (token.equals("NOTOKENS")) {
            if (datatype == NxsCharactersBlock::continuous) {
                throw NxsException("NOTOKENS is not allowed for the CONTINUOUS datatype", token.getFilePosition(), token.getFileLine(), token.getFileColumn());
            }
            tokens = false;

            nxs->NxsLogMesssage(
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
        throw NxsException("TOKENS must be defined for DATATYPE=CONTINUOUS", token.getFilePosition(), token.getFileLine(), token.getFileColumn());
    }
    if (tokens && (datatype == dna || datatype == rna || datatype == nucleotide)) {
        throw NxsException("TOKENS not allowed for the DATATYPEs DNA, RNA, or NUCLEOTIDE", token.getFilePosition(), token.getFileLine(), token.getFileColumn());
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
void NxsCharactersBlock::handleEliminate(NxsToken &token)
{
    // Construct an object of type NxsSetReader, then call its run function
    // to store the set in the eliminated set
    NxsSetReader setReader(token, ncharTotal, eliminated, *this, NxsSetReader::charset);
    setReader.run();

    Q_ASSERT(eliminated.count() <= ncharTotal);

    nchar = ncharTotal - eliminated.count();

    if (nchar != ncharTotal && (charLabels.count() > 0 || charStates.count() > 0)) {
        throw NxsException("The ELIMINATE command must appear before character (or character state) labels are specified", token.getFilePosition(), token.getFileLine(), token.getFileColumn());
    }

    if (!charPos.empty()) {
        throw NxsException("Only one ELIMINATE command is allowed, and it must appear before the MATRIX command", token.getFilePosition(), token.getFileLine(), token.getFileColumn());
    }

    buildCharPosArray(true);
}

// Called when TAXLABELS command needs to be parsed from within the CHARACTERS block. Deals with everything after the
// token TAXLABELS up to and including the semicolon that terminates the TAXLABELS command.
void NxsCharactersBlock::handleTaxlabels(NxsToken &token)
{
    if (!newtaxa){
        errorMessage = "NEWTAXA must have been specified in DIMENSIONS command to use the TAXLABELS command in a ";
        errorMessage += blockID;
        errorMessage += " block";
        throw NxsException(errorMessage, token.getFilePosition(), token.getFileLine(), token.getFileColumn());
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
                throw NxsException("Number of taxon labels exceeds NTAX specified in DIMENSIONS command", token.getFilePosition(), token.getFileLine(), token.getFileColumn());
            }
            taxaBlock->addTaxonLabel(token.getToken());
        }
    }

    // NB: Some may object to setting newtaxa to false here, because then the fact that new taxa were
    // specified in this CHARACTERS block rather than in a preceding TAXA block is lost. This will only be
    // important if we wish to recreate the original data file, which I don't anticipate anyone doing with
    // this code (too difficult to remember all comments, the order of blocks in the file, etc.) - better to
    // get a copy of the file data from the QString NxsToken::nexusData and output it somewhere.
    newtaxa = false;
}


/*------------------------------------------------------------------------------------/
 * Other Functions
 *-----------------------------------------------------------------------------------*/

// This virtual function must be overridden for each derived class to provide the ability to return a standard data object.
QMap<QString, QVariant> NxsCharactersBlock::getData()
{
    blockData.insert("NCHAR", nchar);
    return blockData;
}

// Flushes taxonLabels and sets taxaNumber to 0 in preparation for reading a new TAXA block.
void NxsCharactersBlock::reset()
{
    NxsBlock::reset();

    ncharTotal = 0;
    nchar = 0;
    ntaxTotal = 0;
    ntax = 0;
    newchar = true;
    newtaxa = false;
    interleaving = false;
    transposing = false;
    respectingCase = false;
    labels = true;
    tokens = false;
    datatype = NxsCharactersBlock::standard;

    missing = nxs->defaultMissingCharacter;
    gap = nxs->defaultGapCharacter;
    matchchar = nxs->defaultMatchCharacter;

    resetSymbols();
    equates.clear();

    items.clear();
    items.append("STATES");

    statesFormat = STATES_PRESENT;
}

// Returns true if `ch' can be found in the `symbols' list. The value of `respectingCase' is used to determine
// whether or not the search should be case sensitive. Assumes `symbols' is non-NULL.
bool NxsCharactersBlock::isInSymbols(QChar ch)
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
void NxsCharactersBlock::resetSymbols()
{
    symbols.clear();

    switch(datatype)
    {
        case NxsCharactersBlock::dna:
            // "ACGT"
            for(int i = 0; i < nxs->defaultDNAStateSet.count(); i++)
            {
                symbols.append(nxs->defaultDNAStateSet.at(i).at(0));
            }
            break;

        case NxsCharactersBlock::rna:
            // "ACGU";
            for(int i = 0; i < nxs->defaultRNAStateSet.count(); i++)
            {
                symbols.append(nxs->defaultRNAStateSet.at(i).at(0));
            }
            break;

        case NxsCharactersBlock::nucleotide:
            // "ACGT"
            for(int i = 0; i < nxs->defaultNucleotideStateSet.count(); i++)
            {
                symbols.append(nxs->defaultNucleotideStateSet.at(i).at(0));
            }
            break;

        case NxsCharactersBlock::protein:
            // "ACDEFGHIKLMNPQRSTVWY*"
            for(int i = 0; i < nxs->defaultProteinStateSet.count(); i++)
            {
                symbols.append(nxs->defaultProteinStateSet.at(i).at(0));
            }
            break;

        default:
            // "01"
            for(int i = 0; i < nxs->defaultStandardStateSet.count(); i++)
            {
                symbols.append(nxs->defaultStandardStateSet.at(i).at(0));
            }
            break;
    }

    equates.clear();
    getDefaultEquates();
}

QMap<QString, QString> NxsCharactersBlock::getDefaultEquates()
{
    QMap<QString, QString> equatesMap;
    QStringList equatesList;

    switch(datatype)
    {
        case NxsCharactersBlock::dna:
            equatesList = nxs->defaultDNAEquateStates;
            break;

        case NxsCharactersBlock::rna:
            equatesList = nxs->defaultRNAEquateStates;
            break;

        case NxsCharactersBlock::nucleotide:
            equatesList = nxs->defaultNucleotideEquateStates;
            break;

        case NxsCharactersBlock::protein:
            equatesList = nxs->defaultProteinEquateStates;
            break;

        default:
            return equatesMap;
            break;
    }

    // Create equatesMap from equatesList;
    if (equatesList.count() > 0) {
        for(int i = 0; i < equatesList.count(); i++)
        {
            QRegExp rx("(=)");
            QStringList pair = equatesList.at(i)
                    .trimmed()
                    .replace(" ","")
                    .split(rx);
            equatesMap.insert(pair[0],pair[1]);
        }
    }
    if (equatesMap.count() > 0) {
        // Create lowercase keys of the stored uppercase keys/value pairs
        QList<QString> keys = equatesMap.keys();
        for(int i = 0; i < keys.count(); i++) {
            QString uppercase, lowercase;
            uppercase = keys.at(i);
            lowercase = uppercase.toLower();
            equatesMap.insert(lowercase, equatesMap.value(uppercase));
        }

    }
    return equatesMap;
}

// Converts a taxon label to a number corresponding to the taxon's position within the list maintained by the
// NxsTaxaBlock object. This method overrides the virtual function of the same name in the NxsBlock base class. If
// `str' is not a valid taxon label, returns the value 0.
int NxsCharactersBlock::taxonLabelToNumber(QString str)
{
    int i;
    try {
        i = 1 + taxaBlock->findTaxon(str);
    } catch(NxsTaxaBlock::NxsX_NoSuchTaxon){
        i = 0;
    }

    return i;
}

// Converts a character label to a 1-offset number corresponding to the character's position within `charLabels'. This
// method overrides the virtual function of the same name in the NxsBlock base class. If `str' is not a valid character
// label, returns the value 0.
int NxsCharactersBlock::charLabelToNumber(QString str)
{
    int k = 0;
    for (int i = 0; i < charLabels.count(); ++i)
    {
        if (charLabels.at(i) == str){
            return i+1;
        }
    }

    return k;
}

// Use to allocate memory for (and initialize) `charPos' array, which keeps track of the original character index in
// cases where characters have been eliminated. This function is called by handleEliminate in response to encountering
// an ELIMINATE command in the data file, and this is probably the only place where buildCharPosArray should be called
// with `checkEliminated' true. buildCharPosArray is also called in handleMatrix, handleCharstatelabels,
// handleStatelabels, and handleCharlabels.
void NxsCharactersBlock::buildCharPosArray(bool checkEliminated)
{
    charPos.clear();

    int k = 0;
    for (int j = 0; j < ncharTotal; j++)
    {
        if (checkEliminated && isEliminated(j)){
            charPos[j] = INT_MAX;
        } else {
            charPos[j] = k++;
        }
    }
}

// Returns true if character number `origCharIndex' was eliminated, false otherwise. Returns false immediately if
// `eliminated' set is empty.
bool NxsCharactersBlock::isEliminated(int origCharIndex)
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
