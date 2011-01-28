/*************************************************\
| (c) 2010-2011 Stitch Works Software             |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "savefile.h"
#include "updater.h"

class ChartTab;

namespace Ui {
    class MainWindow;
    class StitchLibraryDialog;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0, QString fileName = "");
    ~MainWindow();

    bool hasTab();
    void setupNewTabDialog();

protected:
    void closeEvent(QCloseEvent *event);
    
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

    void documentNewChart();
    void createChart();

    void menuChartAboutToShow();
    void chartEditName();

    void toolsOptions();
    void toolsRegisterSoftware();
    void toolsStitchLibrary();
    void toolsCheckForUpdates();

    void helpAbout();
    
private:
    void setupMenus();
    void setupStitchPalette();
    void readSettings();
    void trialVersionMessage();

    void checkUpdates();
    
    ChartTab* curChartTab();
   
    Ui::MainWindow *ui;
    Ui::StitchLibraryDialog *sld;

    SaveFile *mFile;
    Updater *mUpdater;

    QMap<QString, int> mPatternStitches;
    QMap<QString, int> mPatternColors;

};

#endif // MAINWINDOW_H
