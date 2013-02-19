#ifndef EXTRACTIONWIDGET_H
#define EXTRACTIONWIDGET_H

#include <QWidget>

#include <QuunRarJob.h>

namespace Ui {
class ExtractionWidget;
}

class QuunRar;

class ExtractionWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit ExtractionWidget(QWidget *parent = 0);
    ~ExtractionWidget();
    
    void extractArchive(QuunRar *archive);
    void testArchive(QuunRar *archive);

signals:
    void taskFinished();

private slots:
    void processData(int);
    void changeVolume(QString volume);
    void changeCurrentFile(QString file);
    void finish();
    void error();

private:
    Ui::ExtractionWidget *ui;
    QuunRar *m_archive;
    bool m_working;
};

#endif // EXTRACTIONWIDGET_H
