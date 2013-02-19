#ifndef QUUNRARJOB_H
#define QUUNRARJOB_H

#include <QtCore/QThread>

class QuunRarNextVolumeAsker : public QObject
{
    Q_OBJECT
public:
    QuunRarNextVolumeAsker(QObject *parent);
    Q_INVOKABLE virtual void askUserForNextVolume(const QString &volumeName) = 0;

signals:
    void volumeSelected(QString);
};

class QuunRar;

class QuunRarJobPrivate;
class QuunRarJob : public QThread
{
    Q_OBJECT
public:
    enum Task {
        ExtractTask,
        TestTask
    };

    ~QuunRarJob();

    QuunRar *archive() const;

    unsigned int processedData() const;
    unsigned int totalData() const;

    Task task() const;
    void setTask(Task task);

    QString currentVolume() const;
    QString currentFileName() const;

    void setNextVolumeAsker(QuunRarNextVolumeAsker *asker);

signals:
    void dataProcessed(int totalProcessedData);
    void finished();
    void volumeChanged(QString currentVolume);
    void volumeMissing(QString missingVolume);
    void currentFileChanged(QString fileName);
    void error();

private slots:
    void setCurrentVolumeName(const QString &fileName);

private:
    friend class QuunRar;
    friend class QuunRarPrivate;

    friend int changedVolumeHelper(char *ArcName, int Mode);
    friend int processDataHelper(unsigned char *Addr, int Size);

    void run();

    int processData(unsigned char *Addr, int Size);
    int changedVolume(char *ArcName, int Mode);

    void quitOnError();
    void setCurrentFileName(const QString &fileName);

    QuunRarJob(QuunRar *parent = 0);
    QuunRarJobPrivate *d;

    Q_DISABLE_COPY(QuunRarJob)
};

#endif // QUUNRARJOB_H
