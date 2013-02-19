#include "extractionwidget.h"
#include "ui_extractionwidget.h"

#include <QuunRar.h>

#include <QFileDialog>
#include <QStandardPaths>
#include <QMessageBox>
#include <QDebug>

class GetNextVolumeNameAsker : public QuunRarNextVolumeAsker
{
public:
    GetNextVolumeNameAsker(QObject *parent);
    void askUserForNextVolume(const QString &volumeName);
};

GetNextVolumeNameAsker::GetNextVolumeNameAsker(QObject *parent) :
    QuunRarNextVolumeAsker(parent)
{
}

void GetNextVolumeNameAsker::askUserForNextVolume(const QString &volumeName)
{
    const QString home = QStandardPaths::standardLocations(QStandardPaths::HomeLocation).first();
    emit volumeSelected(QFileDialog::getOpenFileName(static_cast<QWidget *>(parent()),
                                                     tr("Location volume %1").arg(volumeName),
                                                     home,
                                                     tr("RAR File (*.rar)")));
}

ExtractionWidget::ExtractionWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ExtractionWidget)
{
    ui->setupUi(this);
}

ExtractionWidget::~ExtractionWidget()
{
    delete ui;
}

void ExtractionWidget::extractArchive(QuunRar *archive)
{
    if(m_working)
        return;

    m_working = true;
    m_archive = archive;

    ui->labelTask->setText(tr("Extracting:"));
    ui->labelCurrentVolume->setText(m_archive->fileName());
    ui->progressBar->setMaximum(m_archive->totalUnpackedSize());
    ui->labelDestinationFolder->setText(m_archive->actualDestinationDir());
    ui->labelTo->setVisible(true);
    ui->labelDestinationFolder->setVisible(true);

    QuunRarJob *job= m_archive->extract();
    job->setNextVolumeAsker(new GetNextVolumeNameAsker(this));
    connect(job, SIGNAL(dataProcessed(int)), this, SLOT(processData(int)));
    connect(job, SIGNAL(volumeChanged(QString)), this, SLOT(changeVolume(QString)));
    connect(job, SIGNAL(currentFileChanged(QString)), this, SLOT(changeCurrentFile(QString)));
    connect(job, SIGNAL(error()), this, SLOT(error()));
    connect(job, SIGNAL(finished()), this, SLOT(finish()));
}

void ExtractionWidget::testArchive(QuunRar *archive)
{
    if(m_working)
        return;

    m_working = true;
    m_archive = archive;

    ui->labelTask->setText(tr("Testing:"));
    ui->labelCurrentVolume->setText(m_archive->fileName());
    ui->progressBar->setMaximum(m_archive->totalUnpackedSize());
    ui->labelTo->setVisible(false);
    ui->labelDestinationFolder->setVisible(false);

    QuunRarJob *job= m_archive->testFiles();
    job->setNextVolumeAsker(new GetNextVolumeNameAsker(this));
    connect(job, SIGNAL(dataProcessed(int)), this, SLOT(processData(int)));
    connect(job, SIGNAL(volumeChanged(QString)), this, SLOT(changeVolume(QString)));
    connect(job, SIGNAL(currentFileChanged(QString)), this, SLOT(changeCurrentFile(QString)));
    connect(job, SIGNAL(error()), this, SLOT(error()));
    connect(job, SIGNAL(finished()), this, SLOT(finish()));
}

void ExtractionWidget::processData(int data)
{
    ui->progressBar->setValue(data);
}

void ExtractionWidget::changeVolume(QString volume)
{
    ui->labelCurrentVolume->setText(volume);
}

void ExtractionWidget::changeCurrentFile(QString file)
{
    ui->labelCurrentFile->setText(file);
}

void ExtractionWidget::finish()
{
    ui->labelCurrentVolume->setText(tr("Done"));
    ui->labelCurrentFile->setText(tr(""));

    // Delete job later
    sender()->deleteLater();

    m_working = false;
    emit taskFinished();
}

void ExtractionWidget::error()
{
    QMessageBox msg;
    msg.setIcon(QMessageBox::Warning);
    msg.setText("Extract failed");
    msg.setInformativeText(QString::fromLatin1("%1 (%2)")
                           .arg(m_archive->errorString())
                           .arg(m_archive->error()));
    msg.exec();

    // Delete job later
    sender()->deleteLater();

    m_working = false;
    emit taskFinished();
}

