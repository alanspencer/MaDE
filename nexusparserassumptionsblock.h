#ifndef NEXUSPARSERASSUMPTIONSBLOCK_H
#define NEXUSPARSERASSUMPTIONSBLOCK_H

#include <QtWidgets>

class NexusParserReader;
class NexusParserBlock;
class NexusParserTaxaBlock;
class NexusParserException;

class NexusParserAssumptionsBlock : public NexusParserBlock
{
public:
    NexusParserAssumptionsBlock(NexusParserReader *pointer, NexusParserTaxaBlock *tBlock);
};

#endif // NEXUSPARSERASSUMPTIONSBLOCK_H
