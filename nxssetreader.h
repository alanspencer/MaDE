#ifndef NXSSETREADER_H
#define NXSSETREADER_H

class NxsToken;
class NxsBlock;

class NxsSetReader
{
public:
    NxsSetReader(NxsToken &t, int maxValue, NxsIntSetMap &ism, NxsBlock &b, int type);

    enum NxsSetReaderEnum           // For use with the variable `settype'
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

    NxsToken &token;        // reference to the token being used to parse the NEXUS data file
    NxsBlock &block;        // reference to the block object used for looking up labels
    int max;                // maximum number of elements in the set
    NxsIntSetMap &intSetMap;
    int settype;            // the type of set being read (see the NxsSetReaderEnum enumeration)
};

#endif // NXSSETREADER_H
