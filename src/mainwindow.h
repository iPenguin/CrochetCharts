/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "savefile.h"
#include "updater.h"
#include <qundogroup.h>

class ChartTab;
class QPrinter;

namespace Ui {
    class MainWindow;
    class StitchLibraryDialog;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    friend class SaveFile;
public:
    explicit MainWindow(QWidget *parent = 0, QString fileName = "");
    ~MainWindow();

    bool hasTab();
    void setupNewTabDialog();

protected:
    void closeEvent(QCloseEvent *event);

    //generate the next "new chart" name ie: chart, chart1, chart2...
    QString nextChartName(QString baseName = tr("Chart"));
    bool docHasChartName(QString name);
    
protected slots:
    void updatePatternStitches();
    void updatePatternColors();

    void tabChanged(int newTab);
    
//menu functions/slots:
private slots:
    void menuFileAboutToShow();
    void fileNew();
    void fileOpen();
    void fileSave();
    void fileSaveAs();
    void fileExport();
    void filePrint();
    void filePrintPreview();

    void menuEditAboutToShow();
    
    void menuViewAboutToShow();
    void viewShowStitches();
    void viewShowPatternColors();
    void viewShowPatternStitches();
    void viewFullScreen(bool state);
    void viewZoomIn();
    void viewZoomOut();

    void menuDocumentAboutToShow();
    void documentNewChart();
    void createChart();
    void removeCurrentTab();
    void removeTab(int tabIndex);

    void menuChartAboutToShow();
    void chartEditName();

    void toolsOptions();
    void toolsRegisterSoftware();
    void toolsStitchLibrary();
    void toolsCheckForUpdates();

    void helpAbout();

private slots:
    void print(QPrinter *printer);
    
private:
    void setupMenus();
    void setupStitchPalette();
    void readSettings();
    void trialVersionMessage();

    void checkUpdates();
    
    void exportPdf(QString selection, QString fileName, QSize size, int resolution);
    void exportSvg(QString selection, QString fileName, QSize size);
    void exportImg(QString selection, QString fileName, QSize size, int resolution);
    
    ChartTab* curChartTab();
   
    Ui::MainWindow *ui;
    Ui::StitchLibraryDialog *sld;

    SaveFile *mFile;
    Updater *mUpdater;

//for the savefile class:
protected:
    QMap<QString, QMap<QString, int> > patternColors() { return mPatternColors; }
    QTabWidget* tabWidget();
    
private:
    QMap<QString, int> mPatternStitches;
    QMap<QString, QMap<QString, int> > mPatternColors;


    QAction *mActionUndo,
            *mActionRedo;
    QUndoGroup mUndoGroup;
};

#endif // MAINWINDOW_H
