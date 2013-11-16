#ifndef NXSSETREADER_H
#define NXSSETREADER_H

#include <QtWidgets>

class NexusParserToken;
class NexusParserBlock;

class NexusParserSetReader
{
public:
    NexusParserSetReader(NexusParserToken &t, int maxValue, QMap<int, int> &ism, NexusParserBlock &b, int type);

    enum NexusParserSetReaderEnum           // For use with the variable `settype'
    {
        generic = 1,                // means expect a generic set (say, characters weights)
        charset,                    // means expect a character set
        taxset                      // means expect a taxon set
    };

    bool run();

protected:
    bool addRange(int first, int last, int modulus = 0);

private:
    int getTokenValue();

    NexusParserToken &token;        // reference to the token being used to parse the NEXUS data file
    NexusParserBlock &block;        // reference to the block object used for looking up labels
    int max;                // maximum number of elements in the set
    QMap<int, int> &intSetMap;// QMap<int, int> of eliminate state
    int settype;            // the type of set being read (see the NexusParserSetReaderEnum enumeration)
};

#endif // NXSSETREADER_H
