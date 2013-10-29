#include "nexusreaderexception.h"

NexusReaderException::NexusReaderException(QString str, qint64 fPos, qint64 fLine, qint64 fCol)
{
    filePos = fPos;
    fileLine = fLine;
    fileCol = fCol;
    msg = str;
}


/*----------------------------------------------------------------------------------------------------------------------
*	Creates a NexusException object with the specified message, getting file position information from the NexusToken.
*---------------------------------------------------------------------------------------------------------------------*/
NexusReaderException::NexusReaderException(const QString &str, const NexusReaderToken &token)
{
    msg = str;
    filePos = token.getFilePosition();
    fileLine = token.getFileLine();
    fileCol = token.getFileColumn();
}

