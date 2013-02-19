#ifndef QUUNRARFILEINFO_H
#define QUUNRARFILEINFO_H

#include <QtCore/QObject>

#include <QtCore/QSharedData>

struct RARHeaderData;

class QuunRarFilePrivate;
class QuunRarFileInfo
{
public:
    enum HostOs {
        MsDos = 0,
        Os2 = 1,
        Win32 = 2,
        Unix = 3,
        UnknownHostOs = 4
    };

    QuunRarFileInfo();
    ~QuunRarFileInfo();
    QuunRarFileInfo(const QuunRarFileInfo &other);
    QuunRarFileInfo &operator =(const QuunRarFileInfo &other);

    QString fileName() const;
    unsigned int packedSize() const;
    unsigned int unpackedSize() const;
    HostOs hostOs() const;
    unsigned int crc() const;
    unsigned int fileTime() const;
    unsigned int unpackVersion() const;
    unsigned int packingMethod() const;
    unsigned int fileAttributes() const;
    QString comment() const;
    bool passwordNeeded() const;

private:
    friend class QuunRarPrivate;

    static QuunRarFileInfo fromRarHeaderData(const RARHeaderData &rarHeaderData);

    QSharedDataPointer<QuunRarFilePrivate> d;
};

#endif // QUUNRARFILEINFO_H
