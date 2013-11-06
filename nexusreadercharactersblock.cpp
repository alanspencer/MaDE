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

#include "nexusreadercharactersblock.h"

NexusReaderCharactersBlock::NexusReaderCharactersBlock(NexusReaderTaxaBlock *tBlock)
{
    Q_ASSERT(tBlock != NULL);
    //assert(aBlock != NULL);

    nchar = 0;
    blockID = "CHARACTERS";

    taxaBlock = tBlock;

    //matrix          = NULL;
    //charPos         = NULL;
    //taxonPos        = NULL;
    //activeTaxon     = NULL;
    //activeChar      = NULL;
    symbols.clear();

    reset();
}

NexusReaderCharactersBlock::~NexusReaderCharactersBlock()
{
    reset();

    if(symbols.count() > 0) {
        symbols.clear();
    }
}

// This function provides the ability to read everything following the block name (which is read by the NxsReader
// object) to the END or ENDBLOCK statement. Characters are read from the input stream `in'. Overrides the abstract
// virtual function in the base class.
void NexusReaderCharactersBlock::read(NexusReaderToken *&token)
{
    isEmpty = false;
    requireSemicolonToken(token, QString("BEGIN %1").arg(blockID));

    // Get Taxa Number (nTax)
    ntax = taxaBlock->getNTAX();
    for(;;)
    {
        token->getNextToken();
        NexusReaderBlock::NexusCommandResult result = handleBasicBlockCommands(token);
        if (result == NexusReaderBlock::NexusCommandResult(STOP_PARSING_BLOCK)){
            return;
        }
        if (result != NexusReaderBlock::NexusCommandResult(HANDLED_COMMAND)) {
            if (token->equals("DIMENSIONS")) {
                handleDimensions(token, "NEWTAXA", "NTAX", "NCHAR");
            } else if (token->equals("FORMAT")) {
                handleFormat(token);
            } else if (token->equals("ELIMINATE")) {
                //handleEliminate(token);
            } else if (token->equals("TAXLABELS")) {
                //handleTaxlabels(token);
            } else if (token->equals("CHARSTATELABELS")) {
                //handleCharstatelabels(token);
            } else if (token->equals("CHARLABELS")) {
                //handleCharlabels(token);
            } else if (token->equals("STATELABELS")) {
                //handleStatelabels(token);
            } else if (token->equals("MATRIX")) {
                //handleMatrix(token);
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

// Called when DIMENSIONS command needs to be parsed from within the CHARACTERS block. Deals with everything after
// the token DIMENSIONS up to and including the semicolon that terminates the DIMENSIONs command. `newtaxaLabel',
// `ntaxLabel' and `ncharLabel' are simply "NEWTAXA", "NTAX" and "NCHAR" for this class, but may be different for
// derived classes that use `newtaxa', `ntax' and `nchar' for other things (e.g., ntax is number of populations in
// an ALLELES block)
void NexusReaderCharactersBlock::handleDimensions(NexusReaderToken *&token, QString newtaxaLabel, QString ntaxLabel, QString ncharLabel)
{
    nexusReader->nexusReaderLogMesssage(
                QString("%1 BLOCK: found command \"DIMENSIONS\" on line %2, now looking for \"%3\" or \"%4\" or \"%5\"...")
                .arg(blockID)
                .arg(token->getFileLine())
                .arg(newtaxaLabel)
                .arg(ntaxLabel)
                .arg(ncharLabel)
                );
    for (;;)
    {
        token->getNextToken();

        if (token->equals(newtaxaLabel)) {
            newtaxa = true;

            nexusReader->nexusReaderLogMesssage(
                        QString("%1 BLOCK: found subcommand \"%2\" on line %3.")
                        .arg(blockID)
                        .arg(newtaxaLabel)
                        .arg(token->getFileLine())
                        );
        } else if (token->equals(ntaxLabel)){
            requireEqualsToken(token, "in DIMENSIONS command");
            ntax = requirePositiveToken(token, ntaxLabel);

            nexusReader->nexusReaderLogMesssage(
                        QString("%1 BLOCK: found subcommand \"%2\" on line %3. NTAX = %4.")
                        .arg(blockID)
                        .arg(ntaxLabel)
                        .arg(token->getFileLine())
                        .arg(ntax)
                        );

            if (newtaxa) {
                ntaxTotal = ntax;
            } else {
                ntaxTotal = taxaBlock->getNTAX();
                if (ntaxTotal < ntax){
                    errorMessage = ntaxLabel;
                    errorMessage += " in ";
                    errorMessage += blockID;
                    errorMessage += " block must be less than or equal to NTAX in TAXA block\nNote: one circumstance that can cause this error is \nforgetting to specify ";
                    errorMessage += ntaxLabel;
                    errorMessage += " in DIMENSIONS command when \na TAXA block has not been provided";
                    throw NexusReaderException(errorMessage, token->getFilePosition(), token->getFileLine(), token->getFileColumn());
                }
            }
        } else if (token->equals(ncharLabel)){
            requireEqualsToken(token, "in DIMENSIONS command");
            nchar = requirePositiveToken(token, ncharLabel);
            ncharTotal = nchar;

            nexusReader->nexusReaderLogMesssage(
                        QString("%1 BLOCK: found subcommand \"%2\" on line %3. NCHAR = %4.")
                        .arg(blockID)
                        .arg(ncharLabel)
                        .arg(token->getFileLine())
                        .arg(nchar)
                        );
        } else if (token->equals(";")){
            break;
        }
    }

    if (newtaxa) {
        //taxa->reset();
    }
}

// Called when FORMAT command needs to be parsed from within the CHARACTERS block. Deals with everything after the
// token FORMAT up to and including the semicolon that terminates the FORMAT command.
void NexusReaderCharactersBlock::handleFormat(NexusReaderToken *&token)
{
    nexusReader->nexusReaderLogMesssage(
                QString("%1 BLOCK: found command \"FORMAT\" on line %2...")
                .arg(blockID)
                .arg(token->getFileLine())
                );

    bool standardDataTypeAssumed = false;
    bool ignoreCaseAssumed = false;

    for (;;)
    {
        token->getNextToken();

        if (token->equals("DATATYPE")){
            requireEqualsToken(token, "after keyword DATATYPE");

            // This should be one of the following: STANDARD, DNA, RNA, NUCLEOTIDE, PROTEIN, or CONTINUOUS
            token->getNextToken();

            if (token->equals("STANDARD")) {
                datatype = standard;
            } else if (token->equals("DNA")) {
                datatype = dna;
            } else if (token->equals("RNA")) {
                datatype = rna;
            } else if (token->equals("NUCLEOTIDE")) {
                datatype = nucleotide;
            } else if (token->equals("PROTEIN")) {
                datatype = protein;
            } else if (token->equals("CONTINUOUS")) {
                datatype = continuous;
                statesFormat = INDIVIDUALS;
                items.append("AVERAGE");
            } else {
                errorMessage = token->getToken();
                errorMessage += " is not a valid DATATYPE within a ";
                errorMessage += blockID;
                errorMessage += " block";
                throw NexusReaderException(errorMessage, token->getFilePosition(), token->getFileLine(), token->getFileColumn());
            }

            nexusReader->nexusReaderLogMesssage(
                        QString("%1 BLOCK: found subcommand \"DATATYPE\" on line %2. DATATYPE = %3.")
                        .arg(blockID)
                        .arg(token->getFileLine())
                        .arg(token->getToken())
                        );

            if (standardDataTypeAssumed && datatype != standard) {
                throw NexusReaderException("DATATYPE must be specified first in FORMAT command", token->getFilePosition(), token->getFileLine(), token->getFileColumn());
            }

            //resetSymbols();

            if (datatype == continuous) {
                tokens = true;
            }

        } else if (token->equals("RESPECTCASE")) {
            if (ignoreCaseAssumed) {
                throw NexusReaderException("RESPECTCASE must be specified before MISSING, GAP, SYMBOLS, and MATCHCHAR in FORMAT command", token->getFilePosition(), token->getFileLine(), token->getFileColumn());
            }

            nexusReader->nexusReaderLogMesssage(
                        QString("%1 BLOCK: found subcommand \"RESPECTCASE\" on line %2.")
                        .arg(blockID)
                        .arg(token->getFileLine())
                        );

            standardDataTypeAssumed = true;
            respectingCase = true;
        } else if (token->equals("MISSING")) {
            requireEqualsToken(token, "after keyword MISSING");

            // This should be the missing data symbol (single character)
            token->getNextToken();

            if (token->getTokenLength() != 1){
                errorMessage = "MISSING symbol should be a single character, but ";
                errorMessage += token->getToken();
                errorMessage += " was specified";
                throw NexusReaderException(errorMessage, token->getFilePosition(), token->getFileLine(), token->getFileColumn());
            } else if (token->isPunctuationToken() && !token->isPlusMinusToken()){
                errorMessage = "MISSING symbol specified cannot be a punctuation token (";
                errorMessage += token->getToken();
                errorMessage += " was specified)";
                throw NexusReaderException(errorMessage, token->getFilePosition(), token->getFileLine(), token->getFileColumn());
            } else if (token->isWhitespaceToken()) {
                errorMessage = "MISSING symbol specified cannot be a whitespace character (";
                errorMessage += token->getToken();
                errorMessage += " was specified)";
                throw NexusReaderException(errorMessage, token->getFilePosition(), token->getFileLine(), token->getFileColumn());
            }

            missing = token->getToken().at(0);

            nexusReader->nexusReaderLogMesssage(
                        QString("%1 BLOCK: found subcommand \"MISSING\" on line %2. MISSING = %3.")
                        .arg(blockID)
                        .arg(token->getFileLine())
                        .arg(missing)
                        );

            ignoreCaseAssumed = true;
            standardDataTypeAssumed = true;
        } else if (token->equals("GAP")) {
            requireEqualsToken(token, "after keyword GAP");

            // This should be the gap symbol (single character)
            token->getNextToken();

            if (token->getTokenLength() != 1) {
                errorMessage = "GAP symbol should be a single character, but ";
                errorMessage += token->getToken();
                errorMessage += " was specified";
                throw NexusReaderException(errorMessage, token->getFilePosition(), token->getFileLine(), token->getFileColumn());
            } else if (token->isPunctuationToken() && !token->isPlusMinusToken()) {
                errorMessage = "GAP symbol specified cannot be a punctuation token (";
                errorMessage += token->getToken();
                errorMessage += " was specified)";
                throw NexusReaderException(errorMessage, token->getFilePosition(), token->getFileLine(), token->getFileColumn());
            }  else if (token->isWhitespaceToken()) {
                errorMessage = "GAP symbol specified cannot be a whitespace character (";
                errorMessage += token->getToken();
                errorMessage += " was specified)";
                throw NexusReaderException(errorMessage, token->getFilePosition(), token->getFileLine(), token->getFileColumn());
            }

            gap = token->getToken().at(0);

            nexusReader->nexusReaderLogMesssage(
                        QString("%1 BLOCK: found subcommand \"GAP\" on line %2. GAP = %3.")
                        .arg(blockID)
                        .arg(token->getFileLine())
                        .arg(gap)
                        );

            ignoreCaseAssumed = true;
            standardDataTypeAssumed = true;
        } else if (token->equals("SYMBOLS")) {
            if (datatype == NexusReaderCharactersBlock::continuous) {
                throw NexusReaderException("SYMBOLS subcommand not allowed for DATATYPE=CONTINUOUS", token->getFilePosition(), token->getFileLine(), token->getFileColumn());
            }

            int numDefStates;
            int maxNewStates;
            switch(datatype){
                case NexusReaderCharactersBlock::dna:
                case NexusReaderCharactersBlock::rna:
                case NexusReaderCharactersBlock::nucleotide:
                    numDefStates = 4;
                    maxNewStates = NEXUS_MAX_STATES-4;
                    break;

                case NexusReaderCharactersBlock::protein:
                    numDefStates = 21;
                    maxNewStates = NEXUS_MAX_STATES-21;
                    break;

                default:
                    numDefStates = 0;
                    symbols.clear();
                    maxNewStates = NEXUS_MAX_STATES;
                    break;
            }

            requireEqualsToken(token, " after keyword SYMBOLS");

            // This should be the symbols list
            token->setLabileFlagBit(NexusReaderToken::doubleQuotedToken);
            token->getNextToken();
            token->stripWhitespace();

            int numNewSymbols = token->getTokenLength();

            if (numNewSymbols > maxNewStates){
                errorMessage = "SYMBOLS defines ";
                errorMessage += numNewSymbols;
                errorMessage += " new states but only ";
                errorMessage += maxNewStates;
                errorMessage += " new states allowed for this DATATYPE";
                throw NexusReaderException(errorMessage, token->getFilePosition(), token->getFileLine(), token->getFileColumn());
            }

            QString t = token->getToken();
            int tlen = t.size();

            // Check to make sure user has not used any symbols already in the default symbols list for this data type
            for (int i = 0; i < tlen; i++)
            {
                if (isInSymbols(t.at(i))){
                    errorMessage = "The character ";
                    errorMessage += t[i];
                    errorMessage += " defined in SYMBOLS has already been predefined for this DATATYPE";
                    throw NexusReaderException(errorMessage, token->getFilePosition(), token->getFileLine(), token->getFileColumn());
                }
            }

            // If we've made it this far, go ahead and add the user-defined symbols to the end of the list of predefined symbols
            QString symbolsString;
            for (int i = 0; i < tlen; i++)
            {
                symbols.append(t.at(i));
                symbolsString.append(t.at(i));
            }

            nexusReader->nexusReaderLogMesssage(
                        QString("%1 BLOCK: found subcommand \"SYMBOLS\" on line %2. SYMBOLS = %3.")
                        .arg(blockID)
                        .arg(token->getFileLine())
                        .arg(symbolsString)
                        );

            ignoreCaseAssumed = true;
            standardDataTypeAssumed = true;
        } else if (token->equals("EQUATE")) {
            /*
            if (datatype == NxsCharactersBlock::continuous)
                GenerateNxsException(token, "EQUATE subcommand not allowed for DATATYPE=CONTINUOUS");

            DemandEquals(token, "after keyword EQUATE");

            // This should be a double-quote character
            //
            token.GetNextToken();

            if (!token.Equals("\""))
                {
                errormsg = "Expecting '\"' after keyword EQUATE but found ";
                errormsg += token.GetToken();
                errormsg += " instead";
                throw NxsException(errormsg, token.GetFilePosition(), token.GetFileLine(), token.GetFileColumn());
                }

            // Loop until second double-quote character is encountered
            //
            for (;;)
                {
                token.GetNextToken();
                if (token.Equals("\""))
                    break;

                // If token is not a double-quote character, then it must be the equate symbol (i.e., the
                // character to be replaced in the data matrix)
                //
                if (token.GetTokenLength() != 1)
                    {
                    errormsg = "Expecting single-character EQUATE symbol but found ";
                    errormsg += token.GetToken();
                    errormsg += " instead";
                    throw NxsException(errormsg, token.GetFilePosition(), token.GetFileLine(), token.GetFileColumn());
                    }

                // Check for bad choice of equate symbol
                //
                NxsString t = token.GetToken();
                char ch = t[0];
                bool badEquateSymbol = false;

                // The character '^' cannot be an equate symbol
                //
                if (ch == '^')
                    badEquateSymbol = true;

                // Equate symbols cannot be punctuation (except for + and -)
                //
                if (token.IsPunctuationToken() && !token.IsPlusMinusToken())
                    badEquateSymbol = true;

                // Equate symbols cannot be same as matchchar, missing, or gap
                //
                if (ch == missing || ch == matchchar || ch == gap)
                    badEquateSymbol = true;

                // Equate symbols cannot be one of the state symbols currently defined
                //
                if (IsInSymbols(ch))
                    badEquateSymbol = true;

                if (badEquateSymbol)
                    {
                    errormsg = "EQUATE symbol specified (";
                    errormsg += token.GetToken();
                    errormsg += ") is not valid; must not be same as missing, \nmatchchar, gap, state symbols, or any of the following: ()[]{}/\\,;:=*'\"`<>^";
                    throw NxsException(errormsg, token.GetFilePosition(), token.GetFileLine(), token.GetFileColumn());
                    }

                NxsString k = token.GetToken();
                DemandEquals(token, " in EQUATE definition");

                // This should be the token to be substituted in for the equate symbol
                //
                token.SetLabileFlagBit(NxsToken::parentheticalToken);
                token.SetLabileFlagBit(NxsToken::curlyBracketedToken);
                token.GetNextToken();
                NxsString v = token.GetToken();

                // Add the new equate association to the equates list
                //
                equates[k] = v;
                }

            standardDataTypeAssumed = true;
            */
        } else if (token->equals("MATCHCHAR")) {
            /*DemandEquals(token, "after keyword MATCHCHAR");

            // This should be the matchchar symbol (single character)
            //
            token.GetNextToken();

            if (token.GetTokenLength() != 1)
                {
                errormsg = "MATCHCHAR symbol should be a single character, but ";
                errormsg += token.GetToken();
                errormsg += " was specified";
                throw NxsException(errormsg, token.GetFilePosition(), token.GetFileLine(), token.GetFileColumn());
                }

            else if (token.IsPunctuationToken() && !token.IsPlusMinusToken())
                {
                errormsg = "MATCHCHAR symbol specified cannot be a punctuation token (";
                errormsg += token.GetToken();
                errormsg += " was specified) ";
                throw NxsException(errormsg, token.GetFilePosition(), token.GetFileLine(), token.GetFileColumn());
                }

            else if (token.IsWhitespaceToken())
                {
                errormsg = "MATCHCHAR symbol specified cannot be a whitespace character (";
                errormsg += token.GetToken();
                errormsg += " was specified)";
                throw NxsException(errormsg, token.GetFilePosition(), token.GetFileLine(), token.GetFileColumn());
                }

            matchchar = token.GetToken()[0];

            ignoreCaseAssumed = true;
            standardDataTypeAssumed = true;*/
        } else if (token->equals("LABELS")) {
            //labels = true;
            //standardDataTypeAssumed = true;
        } else if (token->equals("NOLABELS")) {
            //labels = false;
            //standardDataTypeAssumed = true;
        } else if (token->equals("TRANSPOSE")) {
            //transposing = true;
            //standardDataTypeAssumed = true;
        } else if (token->equals("INTERLEAVE")) {
            //interleaving = true;
            //standardDataTypeAssumed = true;
        } else if (token->equals("ITEMS")){
            /*DemandEquals(token, "after keyword ITEMS");
            items.clear();
            // This should be STATES (no other item is supported at this time)
            //
            token.GetNextToken();
            if (datatype == NxsCharactersBlock::continuous)
                {
                NxsString s;
                if (token.Equals("("))
                    {
                    token.GetNextToken();
                    while (!token.Equals(")"))
                        {
                        s = token.GetToken();
                        s.ToUpper();
                        items.push_back(std::string(s.c_str()));
                        token.GetNextToken();
                        }
                    }
                else
                    {
                    s = token.GetToken();
                    s.ToUpper();
                    items.push_back(std::string(s.c_str()));
                    }
                }
            else
                {
                if (!token.Equals("STATES"))
                    GenerateNxsException(token, "Sorry, only ITEMS=STATES is supported for discrete datatypes at this time");
                items = std::vector<std::string>(1, std::string("STATES"));
                }
            standardDataTypeAssumed = true;*/
        } else if (token->equals("STATESFORMAT")) {
            /*DemandEquals(token, "after keyword STATESFORMAT");

            // This should be STATESPRESENT (no other statesformat is supported at this time)
            //
            token.GetNextToken();

            if (token.Equals("STATESPRESENT"))
                statesFormat = STATES_PRESENT;
            else
                {
                if (datatype == NxsCharactersBlock::continuous)
                    {
                    if (token.Equals("INDIVIDUALS"))
                        statesFormat = INDIVIDUALS;
                    else
                        GenerateNxsException(token, "Sorry, only STATESFORMAT=STATESPRESENT or STATESFORMAT=INDIVIDUALS are supported for continuous datatypes at this time");
                    }
                else
                    GenerateNxsException(token, "Sorry, only STATESFORMAT=STATESPRESENT supported for discrete datatypes at this time");
                }
            standardDataTypeAssumed = true;*/
        } else if (token->equals("TOKENS")) {
            //tokens = true;
            //standardDataTypeAssumed = true;
        } else if (token->equals("NOTOKENS")) {
            /*if (datatype == NxsCharactersBlock::continuous) {
                GenerateNxsException(token, "NOTOKENS is not allowed for the CONTINUOUS datatype");
            }
            tokens = false;
            standardDataTypeAssumed = true;*/
        } else if (token->equals(";")) {
            break;
        }
    }

    // Perform some last checks before leaving the FORMAT command
    if (!tokens && datatype == continuous) {
        throw NexusReaderException("TOKENS must be defined for DATATYPE=CONTINUOUS", token->getFilePosition(), token->getFileLine(), token->getFileColumn());
    }
    if (tokens && (datatype == dna || datatype == rna || datatype == nucleotide)) {
        //GenerateNxsException(token, "TOKENS not allowed for the DATATYPEs DNA, RNA, or NUCLEOTIDE");
        throw NexusReaderException("TOKENS not allowed for the DATATYPEs DNA, RNA, or NUCLEOTIDE", token->getFilePosition(), token->getFileLine(), token->getFileColumn());
    }
}

// This virtual function must be overridden for each derived class to provide the ability to return a standard data object.
QMap<QString, QVariant> NexusReaderCharactersBlock::getData()
{
    blockData.insert("NCHAR", nchar);
    return blockData;
}

// Flushes taxonLabels and sets taxaNumber to 0 in preparation for reading a new TAXA block.
void NexusReaderCharactersBlock::reset()
{
    NexusReaderBlock::reset();
    nchar = 0;
}

// Returns true if `ch' can be found in the `symbols' list. The value of `respectingCase' is used to determine
// whether or not the search should be case sensitive. Assumes `symbols' is non-NULL.
bool NexusReaderCharactersBlock::isInSymbols(QChar ch)
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

