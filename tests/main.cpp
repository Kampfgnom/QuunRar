#include <QApplication>

#include "extractionwidget.h"

#include <QuunRar.h>
#include <QuunRarFileInfo.h>

#include <QDebug>
#include <QDir>
#include <QEventLoop>

void testAndExtract(const QString &fileName, const QString &password = QLatin1String(""))
{
    QDir outDir(QApplication::applicationDirPath());
#ifdef Q_OS_MAC
    outDir.cdUp();
    outDir.cdUp();
    outDir.cdUp();
#endif
    QString out = QString::fromLatin1("%1.out").arg(fileName);
    outDir.mkpath(out);
    outDir.cd(out);

    QDir inDir(QApplication::applicationDirPath());
#ifdef Q_OS_MAC
    inDir.cdUp();
    inDir.cdUp();
    inDir.cdUp();
#endif

    qDebug() << "Opening" << fileName;

    QuunRar file(inDir.absoluteFilePath(fileName));
    file.setDestinationDir(outDir.absolutePath());
    if(!password.isEmpty())
        file.setPassword(password);


    if(!file.open()) {
        qWarning() << "File could not be opened:" << fileName;
        qWarning() << file.errorString();
        return;
    }
    qDebug() << "Opened" << fileName;

    qDebug() << "Contents:";
    QList<QuunRarFileInfo> fileInfos = file.listFiles();
    foreach(const QuunRarFileInfo &info, fileInfos) {
        qDebug() << info.fileName();
    }

    qDebug() << "Testing files...";
    if(!file.testFilesBlocking()) {
        qWarning() << "Test failed:" << fileName;
        qWarning() << file.errorString();
    }
    qDebug() << "Test successful.";

    qDebug() << "Extracting to" << out;
    if(!file.extractBlocking()) {
        qWarning() << "File could not be extracted:" << fileName;
        qWarning() << file.errorString();
        return;
    }
    qDebug() << "Done.";
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString normalFile(QLatin1String("testData.rar"));
    QString splitFile(QLatin1String("testData.split.part1.rar"));
    QString passwordFile(QLatin1String("testData.password.rar"));
    QString splitPasswordFile(QLatin1String("testData.password.split.part1.rar"));

    QString password(QLatin1String("testData"));

    // Test synchronous extraction
    testAndExtract(normalFile);
    testAndExtract(splitFile);
    testAndExtract(passwordFile, password);
    testAndExtract(splitPasswordFile, password);

    // Test asynchronous extraction
    QDir outDir(QApplication::applicationDirPath());
#ifdef Q_OS_MAC
    outDir.cdUp();
    outDir.cdUp();
    outDir.cdUp();
#endif
    QString out = QString::fromLatin1("%1.outAsync").arg(normalFile);
    outDir.mkpath(out);
    outDir.cd(out);

    QDir inDir(QApplication::applicationDirPath());
#ifdef Q_OS_MAC
    inDir.cdUp();
    inDir.cdUp();
    inDir.cdUp();
#endif

    qDebug() << "Opening" << normalFile;
    QuunRar file(inDir.absoluteFilePath(normalFile));
    file.setDestinationDir(outDir.absolutePath());
    if(!file.open()) {
        qWarning() << "File could not be opened:" << normalFile;
        qWarning() << file.errorString();
        return -1;
    }
    qDebug() << "Opened" << normalFile;

    ExtractionWidget widget;
    widget.show();

    QEventLoop loop;
    QObject::connect(&widget, SIGNAL(taskFinished()), &loop, SLOT(quit()));

    qDebug() << "Testing async:";
    widget.testArchive(&file);
    loop.exec();
    qDebug() << "Done.";

    qDebug() << "Extracting async:";
    widget.extractArchive(&file);
    qDebug() << "Done.";

    int ret = a.exec();

    return ret;
}
