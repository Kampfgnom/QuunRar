#include "quunrarfileinfo.h"

#include <unrar/dll.hpp>

class QuunRarFilePrivate : public QSharedData
{
public:
    QuunRarFilePrivate();
    ~QuunRarFilePrivate();
    QuunRarFilePrivate( const QuunRarFilePrivate &other );

    QString fileName;
    unsigned int packedSize;
    unsigned int unpackedSize;
    QuunRarFileInfo::HostOs hostOs;
    unsigned int crc;
    unsigned int fileTime;
    unsigned int unpackVersion;
    unsigned int packingMethod;
    unsigned int fileAttributes;
    QString comment;
    bool passwordNeeded;
};

QuunRarFilePrivate::QuunRarFilePrivate() :
    QSharedData(),
    fileName (QLatin1String("")),
    packedSize (0),
    unpackedSize (0),
    hostOs (QuunRarFileInfo::UnknownHostOs),
    crc (0),
    fileTime (0),
    unpackVersion (0),
    packingMethod (0),
    fileAttributes (0),
    comment (QLatin1String("")),
    passwordNeeded(false)
{
}

QuunRarFilePrivate::~QuunRarFilePrivate()
{
}

QuunRarFilePrivate::QuunRarFilePrivate(const QuunRarFilePrivate &other) :
    QSharedData(other),
    fileName (other.fileName),
    packedSize (other.packedSize),
    unpackedSize (other.unpackedSize),
    hostOs (other.hostOs),
    crc (other.crc),
    fileTime (other.fileTime),
    unpackVersion (other.unpackVersion),
    packingMethod (other.packingMethod),
    fileAttributes (other.fileAttributes),
    comment (other.comment),
    passwordNeeded(other.passwordNeeded)
{
}


QuunRarFileInfo::QuunRarFileInfo() :
    d (new QuunRarFilePrivate)
{
}

QuunRarFileInfo::~QuunRarFileInfo()
{
}

QuunRarFileInfo::QuunRarFileInfo(const QuunRarFileInfo &other) :
    d (other.d)
{
}

QuunRarFileInfo &QuunRarFileInfo::operator =(const QuunRarFileInfo &other)
{
    d = other.d;
    return *this;
}

QString QuunRarFileInfo::fileName() const
{
    return d->fileName;
}

unsigned int QuunRarFileInfo::packedSize() const
{
    return d->packedSize;
}

unsigned int QuunRarFileInfo::unpackedSize() const
{
    return d->unpackedSize;
}

QuunRarFileInfo::HostOs QuunRarFileInfo::hostOs() const
{
    return d->hostOs;
}

unsigned int QuunRarFileInfo::crc() const
{
    return d->crc;
}

unsigned int QuunRarFileInfo::fileTime() const
{
    return d->fileTime;
}

unsigned int QuunRarFileInfo::unpackVersion() const
{
    return d->unpackVersion;
}

unsigned int QuunRarFileInfo::packingMethod() const
{
    return d->packingMethod;
}

unsigned int QuunRarFileInfo::fileAttributes() const
{
    return d->fileAttributes;
}

QString QuunRarFileInfo::comment() const
{
    return d->comment;
}

bool QuunRarFileInfo::passwordNeeded() const
{
    return d->passwordNeeded;
}

QuunRarFileInfo QuunRarFileInfo::fromRarHeaderData(const RARHeaderData &rarHeaderData)
{
    QuunRarFileInfo file;
    if (rarHeaderData.CmtState == 0
            || rarHeaderData.CmtState == 1)
        file.d->comment = QString::fromLatin1(rarHeaderData.CmtBuf);

    file.d->crc = rarHeaderData.FileCRC;
    file.d->fileAttributes = rarHeaderData.FileAttr;
    file.d->fileName = QString::fromLatin1(rarHeaderData.FileName);
    file.d->fileTime = rarHeaderData.FileTime;
    file.d->hostOs = static_cast<HostOs>(rarHeaderData.HostOS);
    file.d->packedSize = rarHeaderData.PackSize;
    file.d->packingMethod = rarHeaderData.Method;
    file.d->unpackedSize = rarHeaderData.UnpSize;
    file.d->unpackVersion = rarHeaderData.UnpVer;
    file.d->passwordNeeded = rarHeaderData.Flags & 0x04;
    return file;
}
