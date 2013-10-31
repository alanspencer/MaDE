#include "nexusreaderblock.h"

NexusReaderBlock::NexusReaderBlock()
{
    blockID.clear();
    errorMessage.clear();
}

QString NexusReaderBlock::getID()
{
    return blockID;
}

void NexusReaderBlock::setNexusReader(NexusReader *pointer)
{
    nexusReader = pointer;
}
