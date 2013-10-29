#ifndef NEXUSREADEREXCEPTION_H
#define NEXUSREADEREXCEPTION_H

#include "nexusreader.h"

class NexusReaderToken;

class NexusReaderException
{
    public:
        NexusReaderException(QString s, qint64 fPos = Q_INT64_C(0), qint64 fLine = Q_INT64_C(0), qint64 fCol = Q_INT64_C(0));
        NexusReaderException(const QString &s, const NexusReaderToken &t);

        QString	msg;	/* NexusString to hold message */
        qint64 filePos;	/* current file position */
        qint64 fileLine;	/* current line in file */
        qint64 fileCol;	/* column of current line */
};

#endif // NEXUSREADEREXCEPTION_H
