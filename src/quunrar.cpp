#include "quunrar.h"
#include "quunrar_p.h"

#include "quunrarfileinfo.h"
#include "quunrarjob.h"

#include <unrar/dll.hpp>

#include <QDir>

QuunRarPrivate::QuunRarPrivate() :
    archiveData(new RAROpenArchiveData),
    mutex(QMutex::Recursive)
{
    initValues();
}

QuunRarPrivate::~QuunRarPrivate()
{
    // Destroy RAR-internal data structure
    if(hArcData) {
        RARCloseArchive(hArcData);
        hArcData = nullptr;
    }

    // Quit gracefully
    if(job) {
        job->quit();
        job->deleteLater();
        job = nullptr;
    }

    delete archiveData;
    archiveData = nullptr;
}

void QuunRarPrivate::initValues()
{
    hArcData = NULL;
    archiveData->OpenMode = RAR_OM_EXTRACT;
    archiveData->CmtBufSize = 0;
    archiveData->CmtBuf = NULL;
    archiveData->OpenResult = 0;
    archiveData->CmtState = 0;

    rarFileName = QByteArray();
    destinationDir = QByteArray();

    comment = QLatin1String("");
    password = QByteArray();

    readHeaderResult = 0;
    processResult = 0;
    closeResult = 0;

    fileList = QList<QuunRarFileInfo>();

    passwordNeeded = false;
    archiveVersionTooHigh = false;
    version = 0;

    totalUnpackedSize = 0;
    job = 0;
}

void QuunRarPrivate::cleanUpAfterError()
{
    // Destroy RAR-internal data structure
    if(hArcData) {
        RARCloseArchive(hArcData);
        hArcData = nullptr;
    }

    // Quit running job
    if(job) {
        job->quitOnError();
        job = nullptr;
    }

    delete archiveData;
    archiveData = nullptr;

    // Reset unpacked size
    totalUnpackedSize = 0;
}

bool QuunRarPrivate::open()
{
    if(isOpen())
        return true;

    // Open in extract mode
    archiveData->OpenMode = RAR_OM_EXTRACT;
    archiveData->CmtBufSize = 0;
    archiveData->CmtBuf = NULL;
    hArcData = RAROpenArchive(archiveData);

    if (!isOpen()) {
        cleanUpAfterError();
        return false;
    }

    // Set the password
    if(!password.isEmpty()) {
        RARSetPassword(hArcData, password.data());
    }

    return true;
}

bool QuunRarPrivate::isOpen()
{
    return hArcData
            && archiveData
            && archiveData->OpenResult == 0;
}

void QuunRarPrivate::close()
{
    // Destroy RAR-internal data structure
    if(hArcData) {
        closeResult = RARCloseArchive(hArcData);
        hArcData = NULL;
        if (closeResult != 0) {
            cleanUpAfterError();
        }
    }
}

bool QuunRarPrivate::readMetaData()
{
    // Read meta data first (e.g. the archive's comment)
    archiveData->OpenMode = RAR_OM_LIST;
    archiveData->CmtBufSize = 2 << 16; // 64k buffer
    QScopedPointer<char, QScopedPointerArrayDeleter<char> > cmtBuf(new char[archiveData->CmtBufSize]);
    archiveData->CmtBuf = cmtBuf.data();
    hArcData = RAROpenArchive(archiveData);
    archiveData->CmtBufSize = 0;

    if (!isOpen()) {
        cleanUpAfterError();
        return false;
    }

    // Read archive comment, if existent
    if (archiveData->CmtState == 0
            || archiveData->CmtState == 1)
        comment = QString::fromLatin1(archiveData->CmtBuf);

    // Set the password
    if(!password.isEmpty()) {
        RARSetPassword(hArcData, password.data());
    }

    // Prepare file header data
    RARHeaderData headerData;
    headerData.CmtBufSize = 2 << 16; // 64k buffer
    QScopedPointer<char, QScopedPointerArrayDeleter<char> > cmtBuf2(new char[headerData.CmtBufSize]);
    headerData.CmtBuf = cmtBuf2.data();

    processResult = 0;
    do {
        // Read current file's header
        readHeaderResult = RARReadHeader(hArcData, &headerData);

        // Check if we are at the end of the archive
        bool fileContinuesInNextVolume = headerData.Flags & 0x02;
        if (readHeaderResult == ERAR_END_ARCHIVE) {
            readHeaderResult = 0;
            if(!fileContinuesInNextVolume) {
                break;
            }
        }

        // Check for errors
        if (readHeaderResult != 0) {
            cleanUpAfterError();
            return false;
        }

        // Create file info
        QuunRarFileInfo file = QuunRarFileInfo::fromRarHeaderData(headerData);
        fileList.append(file);

        // Sum up the total unpacked size of all files
        totalUnpackedSize += file.unpackedSize();

        // Remember the maximum version of the packed files (this can be different for different files??? wtf)
        version = qMax(version, file.unpackVersion());

        // Check for needed password
        passwordNeeded |= file.passwordNeeded();

        // Check archive version
        if(file.unpackVersion() > RAR_DLL_VERSION * 10) {
            archiveVersionTooHigh = true;
            cleanUpAfterError();
            return false;
        }

        // Go to next file
        processResult = RARProcessFile(hArcData, RAR_SKIP, NULL, NULL);
    } while (processResult == 0);

    // Check for errors
    if (processResult != 0) {
        cleanUpAfterError();
        return false;
    }

    // Close file
    close();
    return true;
}

bool QuunRarPrivate::checkForNeededPassword()
{
    return !(passwordNeeded
             && password.isEmpty());
}

bool QuunRarPrivate::performTask(int task)
{
    // Open file if it is not open already
    if(!isOpen() && !open()) {
        cleanUpAfterError();
        return false;
    }

    if(!checkForNeededPassword()) {
        cleanUpAfterError();
        return false;
    }

    // Prepare file header data
    RARHeaderData headerData;
    headerData.CmtBufSize = 0;

    processResult = 0;
    do {
        // Check if enough data has been extracted
        if(job && job->processedData() == totalUnpackedSize) {
            break;
        }

        // Read current file's header
        readHeaderResult = RARReadHeader(hArcData, &headerData);

        // Check if we are at the end of the file
        bool fileContinuesInNextVolume = headerData.Flags & 0x02;
        if (readHeaderResult == ERAR_END_ARCHIVE) {
            readHeaderResult = 0;
            if(!fileContinuesInNextVolume) {
                break;
            }
        }

        // Check for errors
        if (readHeaderResult != 0) {
            cleanUpAfterError();
            return false;
        }

        // Set the jobs current file name
        if(job) {
            job->setCurrentFileName(QString::fromLatin1(headerData.FileName));
        }

        // Extract file
        QByteArray destPathCopy = destinationDir;
        char *destPath = destPathCopy.data();
        if (destinationDir.isEmpty())
            destPath = NULL;
        processResult = RARProcessFile(hArcData, task, destPath, NULL);

    } while (processResult == 0);

    // Check for errors
    if (processResult != 0) {
        cleanUpAfterError();
        return false;
    }

    // Close file
    close();

    return true;
}

QuunRar::QuunRar(const QString &fileName, QObject *parent) :
    QObject(parent)
{
    d = new QuunRarPrivate;
    d->q = this;
    d->rarFileName = fileName.toLatin1();
    d->archiveData->ArcName = d->rarFileName.data();
}

QuunRar::~QuunRar()
{
    delete d;
}

bool QuunRar::open()
{
    return d->readMetaData();
}

bool QuunRar::extractBlocking() const
{
    QMutexLocker locker(&d->mutex);
    return d->performTask(RAR_EXTRACT);
}

QuunRarJob *QuunRar::extract()
{
    QMutexLocker locker(&d->mutex);

    // Check if another job is running
    if(d->job) {
        switch(d->job->task()) {
        case QuunRarJob::ExtractTask:
            // An extraction task may be returned
            return d->job;
        default:
        case QuunRarJob::TestTask:
            // You cannot extract while another task is running
            return 0;
        }
    }

    // Create and connect a new background job
    d->job = new QuunRarJob(this);
    connect(d->job, SIGNAL(finished()), this, SLOT(jobFinished()));
    connect(d->job, SIGNAL(error()), this, SLOT(jobFinished()));

    d->job->start();
    return d->job;
}

bool QuunRar::testFilesBlocking() const
{
    QMutexLocker locker(&d->mutex);
    return d->performTask(RAR_TEST);
}

QuunRarJob *QuunRar::testFiles()
{
    QMutexLocker locker(&d->mutex);

    // Check if another job is running
    if(d->job) {
        switch(d->job->task()) {
        case QuunRarJob::TestTask:
            // A testing task may be returned
            return d->job;
        default:
        case QuunRarJob::ExtractTask:
            // You cannot test while another task is running
            return 0;
        }
    }

    // Create and connect a new background job
    d->job = new QuunRarJob(this);
    d->job->setTask(QuunRarJob::TestTask);
    connect(d->job, SIGNAL(finished()), this, SLOT(jobFinished()));
    connect(d->job, SIGNAL(error()), this, SLOT(jobFinished()));

    d->job->start();
    return d->job;
}

QString QuunRar::fileName() const
{
    QMutexLocker locker(&d->mutex);
    return d->rarFileName;
}

QString QuunRar::comment() const
{
    QMutexLocker locker(&d->mutex);
    return d->comment;
}

unsigned int QuunRar::totalUnpackedSize() const
{
    return d->totalUnpackedSize;
}

QString QuunRar::destinationDir() const
{
    QMutexLocker locker(&d->mutex);
    return QString::fromLatin1(d->destinationDir);
}

void QuunRar::setDestinationDir(const QString &path)
{
    QMutexLocker locker(&d->mutex);
    d->destinationDir = path.toLatin1();
}

QString QuunRar::actualDestinationDir() const
{
    QMutexLocker locker(&d->mutex);
    if (d->destinationDir.isEmpty())
        return QDir::currentPath();

    return d->destinationDir;
}

QString QuunRar::password() const
{
    QMutexLocker locker(&d->mutex);
    return QString::fromLatin1(d->password);
}

void QuunRar::setPassword(const QString &password)
{
    QMutexLocker locker(&d->mutex);
    d->password = password.toLatin1();

    if (d->hArcData) {
        RARSetPassword(d->hArcData, d->password.data());
    }
}

bool QuunRar::passwordNeeded() const
{
    QMutexLocker locker(&d->mutex);
    return d->passwordNeeded;
}

bool QuunRar::hasError() const
{
    QMutexLocker locker(&d->mutex);
    return error() != NoError;
}

QList<QuunRarFileInfo> QuunRar::listFiles() const
{
    QMutexLocker locker(&d->mutex);
    return d->fileList;
}

void *QuunRar::archiveData() const
{
    QMutexLocker locker(&d->mutex);
    return d->hArcData;
}

void QuunRar::jobFinished()
{
    QMutexLocker locker(&d->mutex);
    d->job = 0;
}

QuunRar::Error QuunRar::error() const
{
    QMutexLocker locker(&d->mutex);
    if (!d->archiveData
            || d->archiveData->OpenResult != 0)
        return OpeningError;

    if (d->archiveData->CmtState != 0
            && d->archiveData->CmtState != 1)
        return CommentError;

    if (d->processResult != 0)
        return ProcessError;

    if (d->readHeaderResult != 0)
        return ReadHeaderError;

    if (d->passwordNeeded
            && d->password.isEmpty())
        return PasswordNeededError;

    if(d->archiveVersionTooHigh)
        return ArchiveVersionTooHighError;

    return NoError;
}

QString QuunRar::errorString() const
{
    QMutexLocker locker(&d->mutex);
    Error e = error();

    switch (e) {
    case OpeningError:
        if(!d->archiveData)
            return QLatin1String("The archive could not be initialized");

        switch (d->archiveData->OpenResult) {
        case ERAR_NO_MEMORY:
            return QLatin1String("Not enough memory to initialize data structures");
        case ERAR_BAD_DATA:
            return QLatin1String("Archive header broken");
        case ERAR_BAD_ARCHIVE:
            return QLatin1String("File is not valid RAR archive");
        case ERAR_EOPEN:
            return QLatin1String("File open error");
        case 0:
        default:
            return QLatin1String("File could not be opened for an unkown reason");
        }
        break;
    case CommentError:
        switch (d->archiveData->CmtState) {
        case ERAR_NO_MEMORY:
            return QLatin1String("Not enough memory to extract comments");
        case ERAR_BAD_DATA:
            return QLatin1String("Broken comment");
        case ERAR_UNKNOWN_FORMAT:
            return QLatin1String("Unknown comment format");
        case ERAR_SMALL_BUF:
            return QLatin1String("Buffer too small, comments not completely read");
        case 0:
        case 1:
        default:
            return QLatin1String("Comment could not be read for an unkown reason");
        }
        break;
    case ProcessError:
        switch (d->processResult) {
        case ERAR_BAD_DATA:
            return QLatin1String("File CRC error");
        case ERAR_BAD_ARCHIVE:
            return QLatin1String("Volume is not valid RAR archive");
        case ERAR_UNKNOWN_FORMAT:
            return QLatin1String("Unknown archive format");
        case ERAR_EOPEN:
            return QLatin1String("Volume open error");
        case ERAR_ECREATE:
            return QLatin1String("File create error");
        case ERAR_ECLOSE:
            return QLatin1String("File close error");
        case ERAR_EREAD:
            return QLatin1String("Read error");
        case ERAR_EWRITE:
            return QLatin1String("Write error");
        case 0:
        default:
            return QLatin1String("File could not be processed for an unkown reason");
        }
        break;
    case ReadHeaderError:
        switch (d->readHeaderResult) {
        case ERAR_END_ARCHIVE:
            return QLatin1String("End of archive");
        case ERAR_BAD_DATA:
            return QLatin1String("File header broken");
        case 0:
        default:
            return QLatin1String("Header could not be read for an unkown reason");
        }
        break;

    case PasswordNeededError:
        return QLatin1String("Password needed");

    case ArchiveVersionTooHighError:
        return QString::fromLatin1("The version of the archive is too high: Is %1, < %2 supported").arg(d->version).arg(10*RAR_DLL_VERSION);

    case NoError:
    default:
        // No error
        break;
    }


    return QLatin1String("No error");
}
