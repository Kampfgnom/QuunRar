#include "quunrarjob.h"

#include <unrar/dll.hpp>

#include "quunrar.h"
#include "quunrar_p.h"

#include <QEventLoop>
#include <QDebug>

class QuunRarJobPrivate
{
public:
    QuunRarJobPrivate();
    ~QuunRarJobPrivate();

    QuunRar *archive;

    QuunRarNextVolumeAsker *asker;
    QByteArray currentVolume;
    QString currentFileName;
    unsigned int processedData;
    QuunRarJob::Task task;
};

QuunRarJobPrivate::QuunRarJobPrivate() :
    archive (0),
    asker (0),
    processedData (0),
    task (QuunRarJob::ExtractTask)
{
}

QuunRarJobPrivate::~QuunRarJobPrivate()
{
}


QuunRarJob::QuunRarJob(QuunRar *parent) :
    QThread(parent),
    d (new QuunRarJobPrivate)
{
    d->archive = parent;
}

QuunRarJob::~QuunRarJob()
{
    delete d;
}

QuunRar *QuunRarJob::archive() const
{
    return d->archive;
}

unsigned int QuunRarJob::processedData() const
{
    return d->processedData;
}

unsigned int QuunRarJob::totalData() const
{
    return d->archive->totalUnpackedSize();
}

QuunRarJob::Task QuunRarJob::task() const
{
    return d->task;
}

void QuunRarJob::setTask(QuunRarJob::Task task)
{
    d->task = task;
}

QString QuunRarJob::currentVolume() const
{
    return QString::fromLatin1(d->currentVolume);
}

QString QuunRarJob::currentFileName() const
{
    return d->currentFileName;
}

void QuunRarJob::setNextVolumeAsker(QuunRarNextVolumeAsker *asker)
{
    if(d->asker)
        disconnect(d->asker, SIGNAL(volumeSelected(QString)), this, SLOT(setCurrentVolumeName(QString)));

    d->asker = asker;
    connect(d->asker, SIGNAL(volumeSelected(QString)), this, SLOT(setCurrentVolumeName(QString)));
}

int processDataHelper(unsigned char *Addr, int Size) {
    QuunRarJob *job =static_cast<QuunRarJob *>(QThread::currentThread());
    return job->processData(Addr, Size);
}

int QuunRarJob::processData(unsigned char *Addr, int Size)
{
    Q_UNUSED(Addr)

    d->processedData += Size;
    emit dataProcessed(d->processedData);

    if (processedData() == totalData()) {
        emit finished();
        quit();
    }
    return 1; // return non-zero value to continue extraction
}

int changedVolumeHelper(char *ArcName, int Mode) {
    QuunRarJob *job =static_cast<QuunRarJob *>(QThread::currentThread());
    return job->changedVolume(ArcName, Mode);
}

int QuunRarJob::changedVolume(char *ArcName, int Mode)
{
    if (Mode == RAR_VOL_ASK) {
        if(d->asker) {

            // Loop will wait for signal
            QEventLoop loop;
            QObject::connect(d->asker, SIGNAL(volumeSelected(QString)), &loop, SLOT(quit()));

            // Ask user for next volume
            QMetaObject::invokeMethod(d->asker, "askUserForNextVolume", Qt::QueuedConnection,
                                      Q_ARG(QString, QString::fromLatin1(ArcName)));

            loop.exec();

            if(d->currentVolume.isEmpty()) {
                return 0; // abort extraction
            }

            qDebug() << d->currentVolume;
            strcpy(ArcName,d->currentVolume.data());
            emit volumeChanged(QString::fromLatin1(ArcName));
            return 1; // continue with user supplied volume
        }
        else {
            // No volume asker supplied
            emit volumeMissing(QString::fromLatin1(ArcName));
            return 0; // abort extraction
        }
    }
    else if (Mode == RAR_VOL_NOTIFY) {
        d->currentVolume = QByteArray(ArcName);
        emit volumeChanged(QString::fromLatin1(ArcName));
    }

    return 1; // return non-zero value to continue extraction
}

void QuunRarJob::quitOnError()
{
    emit error();
    quit();
}


void QuunRarJob::run()
{
    QMutexLocker locker(&d->archive->d->mutex);
    if(!d->archive->d->open()) {
        emit error();
        return;
    }

    if(!d->archive->d->isOpen()) {
        emit error();
        return;
    }

    RARSetProcessDataProc(d->archive->archiveData(), &processDataHelper);
    RARSetChangeVolProc(d->archive->archiveData(), &changedVolumeHelper);

    switch(d->task) {
    default:
    case ExtractTask:
        d->archive->extractBlocking();
        break;
    case TestTask:
        d->archive->testFilesBlocking();
        break;
    }
}

void QuunRarJob::setCurrentFileName(const QString &fileName)
{
    d->currentFileName = fileName;
    emit currentFileChanged(fileName);
}


QuunRarNextVolumeAsker::QuunRarNextVolumeAsker(QObject *parent) :
    QObject(parent)
{
}


void QuunRarJob::setCurrentVolumeName(const QString &fileName)
{
    d->currentVolume = fileName.toLatin1();
}
