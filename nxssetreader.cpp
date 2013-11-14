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

// Initializes `max' to maxValue, `settype' to `type', `token' to `t', `block' to `block' and `intSetMap' to `ism',
// then clears `nxsset'.
NxsSetReader::NxsSetReader(NxsToken &t, int maxValue, QMap<int, int> &ism, NxsBlock &b, int type) : token(t), block(b), intSetMap(ism)
{
    max = maxValue;
    settype = type;
    intSetMap.clear();
}

// Reads in a set from a NEXUS data file. Returns true if the set was terminated by a semicolon, false otherwise
bool NxsSetReader::run()
{
    bool ok;
    bool returnValue = false;

    int rangeBegin = INT_MAX;
    int rangeEnd = rangeBegin;
    bool insideRange = false;
    int modValue = 1;

    for (;;)
    {
        // Next token should be one of the following:
        //   ';'        --> set definition finished
        //   '-'        --> range being defined
        //   <integer>  --> member of set (or beginning or end of a range)
        //   '.'        --> signifies the number max
        //   '\'        --> signifies modulus value coming next
        token.getNextToken();

        if (token.equals("-")){
            // We should not be inside a range when we encounter a hyphenation symbol. The hyphen is what _puts_ us inside a range!
            if (insideRange){
                block.errorMessage = "The symbol '-' is out of place here.";
                throw NxsException(block.errorMessage,
                                   token.getFilePosition(),
                                   token.getFileLine(),
                                   token.getFileColumn());
            }
            insideRange = true;
        } else if (token.equals(".")) {
            // We _should_ be inside a range if we encounter a period, as this is a range termination character.
            if (insideRange){
                block.errorMessage = "The symbol '.' can only be used to specify the end of a range.";
                throw NxsException(block.errorMessage,
                                   token.getFilePosition(),
                                   token.getFileLine(),
                                   token.getFileColumn());
            }
            rangeEnd = max;
        } else if (token.equals("\\")) {
            // The backslash character is used to specify a modulus to a range, and thus should only be encountered if currently inside a range
            if (insideRange){
                block.errorMessage = "The symbol '\\' can only be used after the end of a range has been specified";
                throw NxsException(block.errorMessage,
                                   token.getFilePosition(),
                                   token.getFileLine(),
                                   token.getFileColumn());
            }
            // This should be the modulus value
            modValue = NxsToken::demandPositiveInt(token, block.errorMessage, "The modulus value");
        } else if (insideRange && rangeEnd == INT_MAX) {
            // The beginning of the range and the hyphen symbol have been read already, just need to store the end of the range at this point
            rangeEnd = getTokenValue();
        } else if (insideRange) {
            // If insideRange is true, we must have already stored the beginning of the range and read in the hyphen character. We would not have
            // made it this far if we had also not already stored the range end. Thus, we can go ahead and add the range.
            ok = addRange(rangeBegin, rangeEnd, modValue);
            modValue = 1;

            if (!ok){
                block.errorMessage = "Character number out of range (or range incorrectly specified) in set specification";
                throw NxsException(block.errorMessage,
                                   token.getFilePosition(),
                                   token.getFileLine(),
                                   token.getFileColumn());
            }

            // We have actually already read in the next token, so deal with it now so that we don't end up skipping a token
            if (token.equals(";")){
                returnValue = true;
                break;
            } else if (token.equals(",")){
                break;
            }

            rangeBegin = getTokenValue();
            rangeEnd = INT_MAX;
            insideRange = false;
        } else if (rangeBegin != INT_MAX) {
            // If we were inside a range, we would have not gotten this far. If not in a range, we are either getting ready to begin a new
            // range or have previously read in a single value. Handle the latter possibility here.
            ok = addRange(rangeBegin, rangeBegin, modValue);
            modValue = 1;

            if (!ok){
                block.errorMessage = "Number out of range (or range incorrectly specified) in set specification";
                throw NxsException(block.errorMessage,
                                   token.getFilePosition(),
                                   token.getFileLine(),
                                   token.getFileColumn());
            }

            // We have actually already read in the next token, so deal with it now so that we don't end up skipping a token
            if (token.equals(";")){
                returnValue = true;
                break;
            } else if (token.equals(",")){
                break;
            }

            rangeBegin = getTokenValue();
            rangeEnd = INT_MAX;
        } else if (token.equals(";")) {
            returnValue = true;
            break;
        } else if (token.equals(",")) {
            break;
        } else if (token.equals("ALL")) {
            rangeBegin = 1;
            rangeEnd = max;
            ok = addRange(rangeBegin, rangeEnd);
        } else {
            // Can only get here if rangeBegin still equals INT_MAX and thus we
            // are reading in the very first token and that token is neither
            // the word "all" nor is it a semicolon
            rangeBegin = getTokenValue();
            rangeEnd = INT_MAX;
        }
    }
    return returnValue;
}

// Adds the range specified by `first', `last', and `modulus' to the set. If `modulus' is zero it is ignored. The
// parameters `first' and `last' refer to numbers found in the data file itself, and thus have range [1..`max']. They
// are stored in `nxsset', however, with offset 0. For example, if the data file says "4-10\2" this function would be
// called with `first' = 4, `last' = 10 and `modulus' = 2, and the values stored in `intSetMap' would be 3, 5, 7, 9. The
// return value is true unless `last' is greater than `max', `first' is less than 1, or `first' is greater than `last':
// in any of these cases, the return value is false to indicate failure to store this range.
bool NxsSetReader::addRange(int first, int last, int modulus)
{
    if (last > max || first < 1 || first > last){
            return false;
    }

    for (int i = first - 1; i < last; i++)
    {
        int diff = i - first + 1;
        if (modulus > 0 && diff % modulus != 0){
            continue;
        }
        intSetMap.insert(i,i);
    }

    return true;
}

// Tries to interpret `token' as a number. Failing that, tries to interpret `token' as a character or taxon label,
// which it then converts to a number. Failing that, it throws a NxsException exception.
int NxsSetReader::getTokenValue()
{
    int v = token.getToken().toInt();

    if (v == 0 && settype != NxsSetReader::generic){
        if (settype == NxsSetReader::charset){
            v = block.charLabelToNumber(token.getToken());
        } else if (settype == NxsSetReader::taxset) {
            v = block.taxonLabelToNumber(token.getToken());
        }
    }

    if (v == 0){
        block.errorMessage = "Set element (";
        block.errorMessage += token.getToken();
        block.errorMessage += ") not a number ";
        if (settype == NxsSetReader::charset) {
            block.errorMessage += "and not a valid character label";
        } else if (settype == NxsSetReader::taxset) {
            block.errorMessage += "and not a valid taxon label";
        }
        throw NxsException(block.errorMessage,
                           token.getFilePosition(),
                           token.getFileLine(),
                           token.getFileColumn());
    }

    return v;
}
