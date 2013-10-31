#ifndef NEXUSREADERBLOCK_H
#define NEXUSREADERBLOCK_H

#include <QWidget>

#include "nexusreader.h"

class NexusReader;

class NexusReaderBlock
{

friend class NexusReader;

public:
    NexusReaderBlock();

    QString getID();
    void setNexusReader(NexusReader *pointer);

    enum NexusCommandResult
    {
        STOP_PARSING_BLOCK,
        HANDLED_COMMAND,
        UNKNOWN_COMMAND
    };

    QString errorMessage;

private:
    NexusReader *nexusReader;
    QString blockID;

};

#endif // NEXUSREADERBLOCK_H
