/*************************************************\
| (c) 2010-2011 Stitch Works Software             |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "savefile.h"

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

private slots:
    void menuFileAboutToShow();
    void fileNew();
    void fileOpen();
    void fileSave();
    void fileSaveAs();
    void fileExport();

    void menuEditAboutToShow();
    
    void menuViewAboutToShow();
    void viewShowStitches();
    void viewFullScreen(bool state);
    void viewZoomIn();
    void viewZoomOut();

    void documentNewChart();
    void createChart();

    void toolsOptions();
    void toolsRegisterSoftware();
    void toolsStitchLibrary();

    void helpAbout();
    
private:
    void setupMenus();
    void setupStitchPalette();
    void readSettings();
    void trialVersionMessage();
    
    ChartTab* curChartTab();
   
    Ui::MainWindow *ui;
    Ui::StitchLibraryDialog *sld;

    SaveFile *mFile;

};

#endif // MAINWINDOW_H
