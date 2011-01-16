/*************************************************\
| (c) 2010-2011 Stitch Works Software             |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
    class MainWindow;
    class StitchLibraryDialog;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    bool hasDocument();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void menuFileAboutToShow();
    void fileOpen();
    void fileSave();
    void fileSaveAs();
    void fileExport();

    void menuEditAboutToShow();
    
    void menuViewAboutToShow();
    void viewShowStitches();
    void viewFullScreen(bool state);

    void documentNewChart();

    void toolsOptions();
    void toolsRegisterSoftware();
    void toolsStitchLibrary();

    void helpAbout();
    
private:
    void setupStitchPalette();
    void readSettings();
    void trialVersionMessage();

    Ui::MainWindow *ui;
    Ui::StitchLibraryDialog *sld;

    void setupMenus();
};

#endif // MAINWINDOW_H
