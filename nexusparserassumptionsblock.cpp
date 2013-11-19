#include "nexusparser.h"

// Sets blockID = "ASSUMPTIONS", taxa = tBlock. Assumes taxa is non-NULL.
NexusParserAssumptionsBlock::NexusParserAssumptionsBlock(NexusParserReader *pointer, NexusParserTaxaBlock *tBlock)
{
    setNexusParserReader(pointer);
    blockID = "ASSUMPTIONS";
    taxaBlock = tBlock;

    standardTypeNames.clear();
    standardTypeNames.append("UNORD");
    standardTypeNames.append("ORD");
    standardTypeNames.append("IRREV");
    standardTypeNames.append("IRREV.UP");
    standardTypeNames.append("IRREV.DOWN");
    standardTypeNames.append("DOLLO.UP");
    standardTypeNames.append("DOLLO.DOWN");
    standardTypeNames.append("STRAT");
    standardTypeNames.append("SQUARED");
    standardTypeNames.append("LINEAR");

    allTypeNames.clear();
    allTypeNames = standardTypeNames;

    userTypeNames.clear();

    polyTCountValue = POLY_T_COUNT_UNKNOWN;
    gapModeValue = GAP_MODE_UNKNOWN;
}

// Nothing needs to be done in the destructor.
NexusParserAssumptionsBlock::~NexusParserAssumptionsBlock()
{
}

// Prepares for reading a new ASSUMPTIONS block. Overrides the pure virtual function in the base class.
void NexusParserAssumptionsBlock::reset()
{
    NexusParserBlock::reset();

    defaultTaxset.clear();
    defaultCharset.clear();
    defaultExset.clear();

    polyTCountValue = POLY_T_COUNT_UNKNOWN;
    gapModeValue = GAP_MODE_UNKNOWN;
}

// A CHARACTERS, DATA, or ALLELES block can call this function to specify that it is to receive notification when the
// current taxon or character set changes (e.g., an "EXSET *" command is read or a program requests that one of the
// predefined taxon sets, character sets, or exsets be applied). Normally, a NexusParserCharactersBlock-derived object calls
// this function upon entering its MATRIX command, since when that happens it becomes the primary data-containing block.
void NexusParserAssumptionsBlock::setCallback(NexusParserCharacterBlock *cBlock)
{
    characterBlock = cBlock;
}

// This function provides the ability to read everything following the block name (which is read by the NexusParser
// object) to the end or ENDBLOCK statement. Characters are read from the input stream in. Overrides the pure virtual
// function in the base class.
void NexusParserAssumptionsBlock::read(NexusParserToken &token)
{
    isEmpty = false;
    demandEndSemicolon(token, "BEGIN ASSUMPTIONS");

    for(;;)
    {
        token.getNextToken();
        NexusParserBlock::NexusParserCommandResult result = handleBasicBlockCommands(token);
        if (result == NexusParserBlock::NexusParserCommandResult(STOP_PARSING_BLOCK)){
            return;
        }
        if (result != NexusParserBlock::NexusParserCommandResult(HANDLED_COMMAND)){
            if (token.equals("OPTIONS")) {
                nexusParser->logMesssage(QString("ASSUMPTIONS Block: found command \"OPTIONS\" on line %1.").arg(token.getFileLine()));
                handleOptions(token);
            } else if (token.equals("EXSET")) {
                nexusParser->logMesssage(QString("ASSUMPTIONS Block: found command \"EXSET\" on line %1.").arg(token.getFileLine()));
                handleExSet(token);
            } else if (token.equals("TAXSET")) {
                nexusParser->logMesssage(QString("ASSUMPTIONS Block: found command \"TAXSET\" on line %1.").arg(token.getFileLine()));
                handleTaxSet(token);
            } else if (token.equals("CHARPARTITION")) {
                nexusParser->logMesssage(QString("ASSUMPTIONS Block: found command \"CHARPARTITION\" on line %1.").arg(token.getFileLine()));
                handleCharPartition(token);
            } else if (token.equals("CHARSET")) {
                nexusParser->logMesssage(QString("ASSUMPTIONS Block: found command \"CHARSET\" on line %1.").arg(token.getFileLine()));
                handleCharSet(token);
            } else if (token.equals("CODESET")) {
                nexusParser->logMesssage(QString("ASSUMPTIONS Block: found command \"CODESET\" on line %1.").arg(token.getFileLine()));
                handleCodeSet(token);
            } else if (token.equals("CODONPOSSET")) {
                nexusParser->logMesssage(QString("ASSUMPTIONS Block: found command \"CODONPOSSET\" on line %1.").arg(token.getFileLine()));
                handleCodonPosSet(token);
            } else if (token.equals("TAXPARTITION")) {
                nexusParser->logMesssage(QString("ASSUMPTIONS Block: found command \"TAXPARTITION\" on line %1.").arg(token.getFileLine()));
                handleTaxPartition(token);
            } else if (token.equals("TREESET")) {
                nexusParser->logMesssage(QString("ASSUMPTIONS Block: found command \"TREESET\" on line %1.").arg(token.getFileLine()));
                handleTreeSet(token);
            } else if (token.equals("TREEPARTITION")) {
                nexusParser->logMesssage(QString("ASSUMPTIONS Block: found command \"TREEPARTITION\" on line %1.").arg(token.getFileLine()));
                handleTreePartition(token);
            } else if (token.equals("TYPESET")) {
                nexusParser->logMesssage(QString("ASSUMPTIONS Block: found command \"TYPESET\" on line %1.").arg(token.getFileLine()));
                handleTypeSet(token);
            } else if (token.equals("USERTYPE")) {
                nexusParser->logMesssage(QString("ASSUMPTIONS Block: found command \"USERTYPE\" on line %1.").arg(token.getFileLine()));
                handleUserType(token);
            } else if (token.equals("WTSET")) {
                nexusParser->logMesssage(QString("ASSUMPTIONS Block: found command \"WTSET\" on line %1.").arg(token.getFileLine()));
                handleWeightSet(token);
            } else {
                skippingCommand(token.getToken());

                do {
                    token.getNextToken();
                } while(!token.getAtEndOfFile() && !token.equals(";"));

                if (token.getAtEndOfFile()){
                    errorMessage = "Unexpected end of file encountered";
                    throw NexusParserException(errorMessage, token);
                }
            }
        }
    }
}

// Reads and stores information contained in the command OPTIONS within an ASSUMPTIONS block.
void NexusParserAssumptionsBlock::handleOptions(NexusParserToken &token)
{
    // Next token should be subcommand e.g. DEFTYPE
    token.getNextToken();

    // Character transformation type
    if (token.equals("DEFTYPE")) {



        demandEquals(token,"after DEFTYPE");

        // "A character type is a specification of the costs and rules imposed on specific state-to-state changes in a parsimony
        // analysis. There are several predefined types: UNORD (unordered),ORD (linearly ordered reversible), IRREV, IRREV.UP,
        // and IRREV.DN (all linearly ordered irreversible), and DOLLO, DOLLO.UP, and DOLLO.DN (variants of Dollo; Swofford, 1989;
        // Maddison and Maddison, 1992). In addition, MacClade has the predefined type STRAT (Fisher's stratigraphic type [Fisher,
        // 1992; Maddison and Maddison, 1992]). UNORD specifies a cost matrix in which all state-to-state changes cost one step; this
        // is the assumption embodied by Fitch parsimony (Fitch, 1971; Hartigan, 1973). ORD specifies a cost of change between two
        // states to be the absolute value of the difference in their state numbers (e.g., a change from state 3 to state 7 costs
        // four steps); this is the assumption in Wagner parsimony (Farris, 1970; Swofford and Maddison, 1987). IRREV and IRREV.UP are
        // equivalent and specify the same cost of change as ORD except that all changes that would involve a decrease in state
        // number are disallowed. IRREV.DN is identical to IRREV except that the changes that are disallowed are those that involve
        // an increase in state number. DOLLO and DOLLO.UP are equivalent and specify the same cost of change as ORD except that each
        // increase in state number can occur only once. DOLLO.DN is identical to DOLLO except that the changes that can only occur
        // once are decreases in state number." - Maddison et al., 1997. Alo there is 'SQUARED' and 'LINEAR' in Mesquite.

        // Therefore next token should be 'UNORD' (default), 'ORD', 'IRREV', 'IRREV.UP', 'IRREV.DN', 'DOLLO', 'DOLLO.UP', 'DOLLO.DN',
        // 'STRAT', 'SQUARED', or 'LINEAR'

        token.getNextToken();

        if (!isValidTypeName(token.getToken())) {
            errorMessage = token.getToken();
            errorMessage += "is not a valid type name for OPTIONS DefType. Expceting one of: ";
            for (int i = 0; i < allTypeNames.count(); i++) {
                errorMessage += allTypeNames[i];
                errorMessage += "; ";
            }
            throw NexusParserException(errorMessage, token);
        } else {
            setDefaultTypeName(token.getToken());
            nexusParser->logMesssage(QString("ASSUMPTIONS Block: found subcommand \"DEFTYPE\" on line %1. DEFTYPE = %2.")
                                     .arg(token.getFileLine())
                                     .arg(token.getToken())
                                     );
        }
    } else if (token.equals("POLYTCOUNT")) {

        demandEquals(token,"after POLYTCOUNT");

        // Next token should be either MINSTEPS or MAXSTEPS
        token.getNextToken();

        if (token.equals("MINSTEPS")) {
            polyTCountValue = POLY_T_COUNT_MIN;
            nexusParser->logMesssage(QString("ASSUMPTIONS Block: found subcommand \"POLYTCOUNT\" on line %1. POLYTCOUNT = MINSTEPS.").arg(token.getFileLine()));
        } else if (token.equals("MAXSTEPS")) {
            polyTCountValue = POLY_T_COUNT_MAX;
            nexusParser->logMesssage(QString("ASSUMPTIONS Block: found subcommand \"POLYTCOUNT\" on line %1. POLYTCOUNT = MAXSTEPS.").arg(token.getFileLine()));
        } else {
            errorMessage = "Unknown value (";
            errorMessage += token.getToken();
            errorMessage += ") found for OPTIONS subcommand POLYTCOUNT (expecting MINSTEPS or MAXSTEPS).";
            throw NexusParserException(errorMessage, token);
        }
    } else if (token.equals("GAPMODE")) {

        demandEquals(token,"after GAPMODE");

        // Next token should be either MISSING (default) or NEWSTATE
        token.getNextToken();

        if (token.equals("MISSING")) {
            gapModeValue = GAP_MODE_MISSING;
            nexusParser->logMesssage(QString("ASSUMPTIONS Block: found subcommand \"GAPMODE\" on line %1. GAPMODE = MISSING.").arg(token.getFileLine()));
        } else if (token.equals("NEWSTATE")) {
            nexusParser->logMesssage(QString("ASSUMPTIONS Block: found subcommand \"GAPMODE\" on line %1. GAPMODE = NEWSTATE.").arg(token.getFileLine()));
            gapModeValue = GAP_MODE_NEWSTATE;
        } else {
            errorMessage = "Unknown value (";
            errorMessage += token.getToken();
            errorMessage += ") found for OPTIONS subcommand GAPMODE (expecting MISSING or NEWSTATE).";
            throw NexusParserException(errorMessage, token);
        }
    }
}

// Reads and stores information contained in the command EXSET within an ASSUMPTIONS block. If EXSET keyword is
// followed by an asterisk, last read NexusParserCharactersBlock or NexusParserCharactersBlock-derived object is
// notified of the characters to be excluded (its ApplyExset function is called).
void NexusParserAssumptionsBlock::handleExSet(NexusParserToken &token)
{

}

// Reads and stores information contained in the command CHARSET within an ASSUMPTIONS block.
void NexusParserAssumptionsBlock::handleCharSet(NexusParserToken &token)
{

}

// Reads and stores information contained in the command CHARPARTITION within an ASSUMPTIONS block.
void NexusParserAssumptionsBlock::handleCharPartition(NexusParserToken &token)
{

}

//Reads and stores information contained in the command TAXSET within an ASSUMPTIONS block.
void NexusParserAssumptionsBlock::handleTaxSet(NexusParserToken &token)
{

}

//Reads and stores information contained in the command TAXPARTITION within an ASSUMPTIONS block.
void NexusParserAssumptionsBlock::handleTaxPartition(NexusParserToken &token)
{

}

//Reads and stores information contained in the command TREESET within an ASSUMPTIONS block.
void NexusParserAssumptionsBlock::handleTreeSet(NexusParserToken &token)
{

}

//Reads and stores information contained in the command TREEPARTITION within an ASSUMPTIONS block.
void NexusParserAssumptionsBlock::handleTreePartition(NexusParserToken &token)
{

}

//Reads and stores information contained in the command USERTYPE within an ASSUMPTIONS block.
void NexusParserAssumptionsBlock::handleUserType(NexusParserToken &token)
{
    token.getNextToken();

    if (token.equals("*")) {
        errorMessage = "An * is ignored in a USERTYPE command";
        nexusParser->logWarning(errorMessage, NexusParserReader::SKIPPING_CONTENT_WARNING, token);
        errorMessage.clear();
        // Skip token...
        token.getNextToken();
    }

    QString userTypeName = token.getToken();

    bool floatMat = false;
    bool csTreeForm = false;

    token.getNextToken();
    if (token.equals("(")) {
        token.getNextToken();
        while (!token.equals(")")) { // loop #1
            // Will now be looking for one of the following subcommands: CHARACTERS, CSTREE, NOTOKENS, REALMATRIX
            if (token.equals("CHARACTERS")){

            } else if (token.equals("CSTREE")) {
                csTreeForm = true;
            } else if (token.equals("NOTOKENS")) {
                errorMessage = "NOTOKENS-style USERTYPES are not supported";
                throw NexusParserException(errorMessage, token);
            } else if (token.equals("REALMATRIX")) {
                floatMat = true;
            } else if (token.equals(";")) {
                errorMessage = "; encountered in USERTYPE command before parentheses were closed";
                throw NexusParserException(errorMessage, token);
            } else if (!(token.equals("STEPMATRIX") || token.equals("TOKENS"))) {
                errorMessage = "Skipping unknown UserType qualifier: ";
                errorMessage += token.getToken();
                nexusParser->logWarning(errorMessage, NexusParserReader::SKIPPING_CONTENT_WARNING, token);
                errorMessage.clear();
            }
            token.getNextToken();
        } // end loop #1
    }

    if (token.equals("STEPMATRIX") || token.equals("REALMATRIX")) {
        errorMessage = "USERTYPE qualifier ";
        errorMessage += token.getToken();
        errorMessage += " should occur in parentheses (";
        errorMessage += token.getToken();
        errorMessage += ")";
        nexusParser->logWarning(errorMessage, NexusParserReader::DEPRECATED_WARNING, token);
        errorMessage.clear();
        // Skip token...
        token.getNextToken();
    }

    demandEquals(token, "in USERTYPE definition");

    // Read data
    if (csTreeForm) {

    } else {
        // BEGIN Read as Stepmatrix section
    }

    token.getNextToken();
    if (!token.equals(";")){
        errorMessage = "Expecting ; at the end of the UserType command. Found ";
        errorMessage += token.getToken();
        errorMessage += "instead.";
        throw NexusParserException(errorMessage, token);
    }
}

//Reads and stores information contained in the command TYPESET within an ASSUMPTIONS block.
void NexusParserAssumptionsBlock::handleTypeSet(NexusParserToken &token)
{

}

//Reads and stores information contained in the command CODESET within an ASSUMPTIONS block.
void NexusParserAssumptionsBlock::handleCodeSet(NexusParserToken &token)
{

}

//Reads and stores information contained in the command WTSET within an ASSUMPTIONS block.
void NexusParserAssumptionsBlock::handleWeightSet(NexusParserToken &token)
{

}

//Reads and stores information contained in the command CODONPOSSET within an ASSUMPTIONS block.
void NexusParserAssumptionsBlock::handleCodonPosSet(NexusParserToken &token)
{

}

// Returns true if 'str' is the name of a known type (standard or user type) -- not case-sensitive.
void NexusParserAssumptionsBlock::setDefaultTypeName(QString str)
{
    if (!(str.length() == 0 || isValidTypeName(str))){
        errorMessage = str;
        errorMessage += " is not the name of a known type (and therefore is not a valid default type)";
        throw NexusParserException(errorMessage);
    }
    defaultType = str;
}

// Returns true if 'str' is the name of a known type (standard or user type) -- not case-sensitive.
bool NexusParserAssumptionsBlock::isValidTypeName(QString str)
{
    return (allTypeNames.count(str.toUpper()) > 0);
}

// Returns true if 'str' is the name of a standard type (standard or user type) -- not case-sensitive.
bool NexusParserAssumptionsBlock::isStandardTypeName(QString str)
{
    return (standardTypeNames.count(str.toUpper()) > 0);
}
