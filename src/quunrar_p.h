#ifndef QUUNRAR_P_H
#define QUUNRAR_P_H

#include <QMutex>
#include <QList>

struct RAROpenArchiveData;
class QuunRarJob;
class QuunRarFileInfo;

class QuunRar;
class QuunRarPrivate
{
public:
    friend class QuunRarJob;

    QuunRarPrivate();
    ~QuunRarPrivate();

    QuunRar *q;

    RAROpenArchiveData *archiveData;
    void *hArcData;
    QByteArray rarFileName;
    QByteArray destinationDir;
    mutable int readHeaderResult;
    mutable int processResult;
    int closeResult;
    QList<QuunRarFileInfo> fileList;
    QString comment;
    QByteArray password;
    unsigned int totalUnpackedSize;
    mutable QMutex mutex;
    QuunRarJob *job;
    bool passwordNeeded;
    bool archiveVersionTooHigh;
    unsigned int version;

    void initValues();
    void cleanUpAfterError();

    bool open();
    bool isOpen();
    void close();

    bool readMetaData();

    bool checkForNeededPassword();

    bool performTask(int task);
};

#endif // QUUNRAR_P_H
