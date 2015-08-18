/****************************************************************************\
 Copyright (c) 2010-2014 Stitch Works Software
 Brian C. Milco <bcmilco@gmail.com>

 This file is part of Crochet Charts.

 Crochet Charts is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Crochet Charts is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Crochet Charts. If not, see <http://www.gnu.org/licenses/>.

 \****************************************************************************/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "filefactory.h"
#include "updater.h"
#include "undogroup.h"

#include "resizeui.h"
#include "aligndock.h"
#include "rowsdock.h"
#include "mirrordock.h"
#include "propertiesdock.h"

#include <QSortFilterProxyModel>

#include <QModelIndex>

#include "scene.h"

class CrochetTab;
class QPrinter;
class QPainter;
class QActionGroup;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    friend class FileFactory;
    friend class File;
    friend class File_v1;
    friend class File_v2;
public:
    explicit MainWindow(QStringList fileNames = QStringList(), QWidget* parent = 0);
    ~MainWindow();
	
	void dropEvent(QDropEvent *e);
	void dragEnterEvent(QDragEnterEvent *e);

    bool hasTab();
    void setupNewTabDialog();

protected:
    void closeEvent(QCloseEvent* event);

    //generate the next "new chart" name ie: chart, chart1, chart2...
    QString nextChartName(QString baseName = tr("Chart"));
    bool docHasChartName(QString name);

    CrochetTab* createTab(Scene::ChartStyle style);
    
/**
 * The proxy icon for the titlebar on Mac OS X
 */
private:
    QIcon fileIcon;

protected slots:
    void updatePatternStitches();
    void updatePatternColors();

    void tabChanged(int newTab);
    
    void newChartUpdateStyle(QString style);

    void propertiesUpdate(QString property, QVariant newValue);
    
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
    void viewShowRowsDock();
    void viewShowAlignDock();
    void viewShowMirrorDock();
    void viewShowProperties();
	void viewShowResizeDock();

    void menuModesAboutToShow();
    
    void documentNewChart();
    void newChart();
    void removeCurrentTab();
    void removeTab(int tabIndex);
	
	void addLayer();
	void removeLayer();
	void mergeLayer();
	void selectLayer(const QModelIndex & index);
	void layerModelChanged(const QModelIndex& index);

    void menuChartAboutToShow();
    void chartEditName();
    void chartsShowChartCenter();
    void chartCreateRows(bool state);

    void menuStitchesAboutToShow();
    void stitchesReplaceStitch();
    void stitchesReplaceColor();

    void menuToolsAboutToShow();
    void toolsOptions();
    void toolsStitchLibrary();
    void toolsCheckForUpdates();

    void helpCrochetHelp();
    void helpAbout();

    void copy();
    void cut();
    void paste();

    void group();
    void ungroup();

private slots:
    void print(QPrinter* printer);

    void changeTabMode(QAction* a);

    void documentIsModified(bool isModified);

    void selectStitch(QModelIndex index);
    void selectColor(QModelIndex index);

    void filterStitchList(QString newText);
    void clearStitchFilter();

    void openRecentFile();

    void saveFileAs(QString fileName);

    void addColor(QColor color);

    void updateDefaultStitchColor(QColor originalColor, QColor newColor);
	
	void reloadLayerContent(QList<ChartLayer*>& layers, ChartLayer* selected);

public slots:
    void loadFile(QString fileName);
private:
    void loadFiles(QStringList fileNames);
    
    void setupMenus();
    void setupRecentFiles();
    void updateMenuItems();
    
    QSortFilterProxyModel *mProxyModel;
    void setupStitchPalette();
	void setupLayersDock();
    void setupDocks();

    /**
     * @brief readSettings - restore geometry settings from the application settings.
     */
    void readSettings();

    void checkUpdates(bool silent = true);

    bool safeToClose();
    bool promptToSave();

    void setEditMode(int mode);

    void setApplicationTitle();

    CrochetTab* curCrochetTab();

    Ui::MainWindow* ui;

    FileFactory* mFile;
    Updater* mUpdater;

//for the savefile class:
protected:
    QMap<QString, int> patternStitches() { return mPatternStitches; }
    QMap<QString, QMap<QString, qint64> > patternColors() { return mPatternColors; }
    QTabWidget* tabWidget();
    void showFileError(int error);

//Flash the new Document dialog when the user selects new doc or new chart.
private slots:
    void flashNewDocDialog();

    void alignSelection(int style);
    void distributeSelection(int style);
    void arrangeGrid(QSize grid, QSize alignment, QSize spacing, bool useSelection);

	void copy(int direction);
    void mirror(int direction);
    void rotate(qreal degrees);
	void resize(QRectF scenerect);

    void updateGuidelines(Guidelines guidelines);
    
private:
    //Save the color of the widget when we flash the New Doc Dialog.
    QColor mNewDocWidgetColor;
    
private:
    QMap<QString, int> mPatternStitches;
    QMap<QString, QMap<QString, qint64> > mPatternColors;

    QActionGroup* mModeGroup;
    
    QAction* mActionUndo,
           * mActionRedo;

    void addToRecentFiles(QString fileName);
    QList<QAction*> mRecentFilesActs;
    
    QDockWidget* mUndoDock;
    UndoGroup mUndoGroup;
	
	ResizeUI* mResizeUI;
    AlignDock* mAlignDock;
    RowsDock* mRowsDock;
    MirrorDock* mMirrorDock;

    PropertiesDock* mPropertiesDock;
    
    int mEditMode;
    QString mStitch;
    QColor mFgColor;
    QColor mBgColor;
};

#endif // MAINWINDOW_H
