#ifndef NEXUSPARSERASSUMPTIONSBLOCK_H
#define NEXUSPARSERASSUMPTIONSBLOCK_H

#include <QtWidgets>

class NexusParserReader;
class NexusParserBlock;
class NexusParserTaxaBlock;
class NexusParserCharacterBlock;
class NexusParserException;
class NexusParserToken;

class NexusParserAssumptionsBlock : public NexusParserBlock
{
public:
    NexusParserAssumptionsBlock(NexusParserReader *pointer, NexusParserTaxaBlock *tBlock);
    virtual ~NexusParserAssumptionsBlock();

    void setCallback(NexusParserCharacterBlock *cBlock);
    void setDefaultTypeName(QString str);

    virtual void reset();

protected:
    virtual void read(NexusParserToken &token);

    void handleOptions(NexusParserToken &token);

    void handleCharSet(NexusParserToken &token);
    void handleCharPartition(NexusParserToken &token);
    void handleExSet(NexusParserToken &token);
    void handleTaxSet(NexusParserToken &token);
    void handleTaxPartition(NexusParserToken &token);
    void handleTreeSet(NexusParserToken &token);
    void handleTreePartition(NexusParserToken &token);
    void handleWeightSet(NexusParserToken &token);
    void handleUserType(NexusParserToken &token);
    void handleCodonPosSet(NexusParserToken &token);
    void handleCodeSet(NexusParserToken &token);
    void handleTypeSet(NexusParserToken &token);

    bool isValidTypeName(QString str);
    bool isStandardTypeName(QString str);

    QString defaultCharset;     // the default charset
    QString defaultTaxset;      // the default taxset
    QString defaultExset;       // the default exset

    QString defaultType;

private:
    QList<QString> standardTypeNames;
    QList<QString> allTypeNames;
    QList<QString> userTypeNames;
    NexusParserTaxaBlock *taxaBlock;                // pointer to NexusParserTaxaBlock
    NexusParserCharacterBlock *characterBlock;      // point to NexusParserCharacterBlock-derived object for callback

    enum PolyTCountValue
    {
        POLY_T_COUNT_UNKNOWN,
        POLY_T_COUNT_MIN,
        POLY_T_COUNT_MAX
    };
    PolyTCountValue polyTCountValue;

    enum GapModeValue
    {
        GAP_MODE_UNKNOWN,
        GAP_MODE_MISSING,
        GAP_MODE_NEWSTATE
    };
    GapModeValue gapModeValue;
};

#endif // NEXUSPARSERASSUMPTIONSBLOCK_H
