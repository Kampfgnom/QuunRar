#ifndef QUUNRAR_H
#define QUUNRAR_H

#include <QtCore/QObject>

class QuunRarFileInfo;
class QuunRarJob;

class QuunRarPrivate;
class QuunRar : public QObject
{
    Q_OBJECT
public:
    enum Error {
        NoError,
        OpeningError,
        CommentError,
        ProcessError,
        ReadHeaderError,
        PasswordNeededError,
        ArchiveVersionTooHighError
    };

    explicit QuunRar(const QString &fileName, QObject *parent = 0);
    ~QuunRar();

    bool open();

    QString fileName() const;

    QString comment() const;

    unsigned int totalUnpackedSize() const;

    QString destinationDir() const;
    void setDestinationDir(const QString &path);

    QString actualDestinationDir() const;

    QString password() const;
    void setPassword(const QString &password);
    bool passwordNeeded() const;

    bool hasError() const;
    Error error() const;
    QString errorString() const;

    QList<QuunRarFileInfo> listFiles() const;

    bool extractBlocking() const;
    QuunRarJob *extract();

    bool testFilesBlocking() const;
    QuunRarJob *testFiles();

    void *archiveData() const;

private Q_SLOTS:
    void jobFinished();

private:
    friend class QuunRarJob;

    QuunRarPrivate *d;
    Q_DISABLE_COPY(QuunRar)
};

#endif // QUUNRAR_H
