/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "savefile.h"
#include "updater.h"
#include "undogroup.h"

#include <QModelIndex>

class QGraphicsScene;
class CrochetTab;
class QPrinter;
class QPainter;
class QActionGroup;

namespace Ui {
    class MainWindow;
    class StitchLibraryDialog;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    friend class SaveFile;
public:
    explicit MainWindow(QStringList fileNames = QStringList(), QWidget *parent = 0);
    ~MainWindow();

    bool hasTab();
    void setupNewTabDialog();

protected:
    void closeEvent(QCloseEvent *event);

    //generate the next "new chart" name ie: chart, chart1, chart2...
    QString nextChartName(QString baseName = tr("Chart"));
    bool docHasChartName(QString name);

    CrochetTab* createTab();
    
protected slots:
    void updatePatternStitches();
    void updatePatternColors();

    void tabChanged(int newTab);
    
//menu functions/slots:
private slots:
    void menuFileAboutToShow();
    void menuRecentFilesAboutToShow();
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
    void viewShowUndoHistory();
    void viewShowMainToolbar();
    void viewShowEditModeToolbar();
    void viewFullScreen(bool state);
    void viewZoomIn();
    void viewZoomOut();

    void menuModesAboutToShow();
    
    void documentNewChart();
    void newChart();
    void removeCurrentTab();
    void removeTab(int tabIndex);

    void menuChartAboutToShow();
    void chartEditName();

    void menuToolsAboutToShow();
    void toolsOptions();
    void toolsRegisterSoftware();
    void toolsStitchLibrary();
    void toolsCheckForUpdates();

    void helpCrochetHelp();
    void helpAbout();

private slots:
    void print(QPrinter *printer);

    void changeTabMode(QAction *a);

    void documentIsModified(bool isModified);

    void selectColor();
    void selectStitch(QModelIndex index);
    void selectColor(QModelIndex index);

    void openRecentFile();
    
private:
    void loadFiles(QStringList fileNames);
    
    void setupMenus();
    void setupRecentFiles(QStringList files = QStringList());
    void saveRecentFiles();
    void updateMenuItems();
    
    void setupStitchPalette();
    void setupUndoView();
    void readSettings();

    void checkUpdates(bool silent = true);

    bool safeToClose();
    bool promptToSave();

    void updateFgColor();
    void updateBgColor();
    void setEditMode(int mode);

    void setApplicationTitle();
    
    QPixmap drawColorBox(QColor color, QSize size);
    
    CrochetTab* curCrochetTab();
   
    Ui::MainWindow *ui;
    Ui::StitchLibraryDialog *sld;

    SaveFile *mFile;
    Updater *mUpdater;

//for the savefile class:
protected:
    QMap<QString, int> patternStitches() { return mPatternStitches; }
    QMap<QString, QMap<QString, qint64> > patternColors() { return mPatternColors; }
    QTabWidget* tabWidget();

//Flash the new Document dialog when the user selects new doc or new chart.
private slots:
    void flashNewDocDialog();
private:
    QColor mNewDocWidgetColor;
    
private:
    QMap<QString, int> mPatternStitches;
    QMap<QString, QMap<QString, qint64> > mPatternColors;

    QActionGroup *mModeGroup;
    
    QAction *mActionUndo,
            *mActionRedo;

    void addToRecentFiles(QString fileName);
    QStringList mRecentFiles;
    QList<QAction*> mRecentFilesActs;
    
    QDockWidget *mUndoDock;
    UndoGroup mUndoGroup;

    int mEditMode;
    QString mStitch;
    QColor mFgColor;
    QColor mBgColor;
};

#endif // MAINWINDOW_H
