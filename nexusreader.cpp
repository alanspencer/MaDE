#include "nexusreader.h"

NexusReader::NexusReader(QString fname, MainWindow *mw, Settings *s)
{
    mainwindow = mw;
    settings = s;
    filename = fname;

    nexusReaderLogMesssage(QString("starting NEXUS Reader on file \"%1\".").arg(filename));
}

bool NexusReader::execute()
{
    QString errorMessage;

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {

        return 1;
    } else {
        nexusReaderLogMesssage(".nex file found and opened in readonly text mode.");

        // Create new NexusReaderToken
        QTextStream i(&file);
        token = new NexusReaderToken(i);

        try
        {
            token->getNextToken();
        }
        catch (NexusReaderException x)
        {
            nexusReaderLogError(token->errorMessage, 0, 0, 0);
            return false;
        }

        if (!token->tokenEquals("#NEXUS")) {
            errorMessage = "Expecting #NEXUS to be the first token in the file, but found \"";
            errorMessage += token->getToken();
            errorMessage += "\" instead";
            nexusReaderLogError(errorMessage, token->getFilePosition(), token->getFileLine(), token->getFileColumn());
            return false;
        }



        nexusReaderLogMesssage("NEXUS Reader has executed and returned with no fatal errors.");
        return true;
    }
}

// Called when an error is encountered in a NEXUS file. Allows program to give user details of the error as well as
// the precise location of the error via the application log.
void NexusReader::nexusReaderLogError(QString message, qint64 filePos, qint64 fileLine, qint64 fileCol)
{
    // Write to main window application log
    mainwindow->logAppend("NEXUS Reader",
                          QString("ERROR \"%1\" @ File Position = %2, File Line = %3, File Column = %4.")
                          .arg(message)
                          .arg(filePos)
                          .arg(fileLine)
                          .arg(fileCol)
                          );
}

// Called when a message is to be logged. Allows program to give user details of the message via the application log.
void NexusReader::nexusReaderLogMesssage(QString message)
{
    // Write to main window application log
    mainwindow->logAppend("NEXUS Reader",QString("%1").arg(message));
}
