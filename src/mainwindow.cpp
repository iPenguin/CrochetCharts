/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "stitchlibraryui.h"
#include "licensewizard.h"
#include "exportui.h"

#include "appinfo.h"
#include "settings.h"
#include "settingsui.h"

#include "crochettab.h"
#include "stitchlibrary.h"
#include "stitchset.h"
#include "stitchpalettedelegate.h"

#include "debug.h"
#include <QDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QColorDialog>

#include <QPrinter>
#include <QPrintDialog>
#include <QPrintPreviewDialog>

#include <QActionGroup>
#include <QCloseEvent>
#include <QUndoStack>
#include <QUndoView>
#include <QTimer>

#include <QSortFilterProxyModel>
#include <QDesktopServices>

MainWindow::MainWindow(QStringList fileNames, QWidget* parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    mUpdater(0),
    mAlignDock(0),
    mRowsDock(0),
    mMirrorDock(0),
    mEditMode(10),
    mStitch("ch"),
    mFgColor(QColor(Qt::black)),
    mBgColor(QColor(Qt::white))
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    ui->setupUi(this);

    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));
    connect(ui->tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(removeTab(int)));
    
    setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

#ifndef APPLE_APP_STORE
    bool checkForUpdates = Settings::inst()->value("checkForUpdates").toBool();
    if(checkForUpdates)
        checkUpdates();
#endif

    setupStitchPalette();
    setupDocks();
    
    mFile = new SaveFile(this);
    loadFiles(fileNames);

    setApplicationTitle();
    setupNewTabDialog();

    setupMenus();
    readSettings();

    QApplication::restoreOverrideCursor();
}

MainWindow::~MainWindow()
{
}

void MainWindow::loadFiles(QStringList fileNames)
{
    
    if(fileNames.count() < 1)
        return;

    if(ui->tabWidget->count() < 1) {
        mFile->fileName = fileNames.takeFirst();
        mFile->load(); //TODO: if !error hide dialog.
        Settings::inst()->files.insert(mFile->fileName.toLower(), this);
        addToRecentFiles(mFile->fileName);
        ui->newDocument->hide();
    }

    foreach(QString fileName, fileNames) {
        QStringList files;
        files.append(fileName);
        MainWindow* newWin = new MainWindow(files);
        newWin->move(x() + 40, y() + 40);
        newWin->show();
        Settings::inst()->files.insert(mFile->fileName.toLower(), newWin);
        addToRecentFiles(fileName);
    }
}

void MainWindow::checkUpdates(bool silent)
{
    if(mUpdater) {
        delete mUpdater;
        mUpdater = 0;
    }
    
    //TODO: check for updates in a separate thread.
    mUpdater = new Updater(this);
    // append the updater to the centralWidget to keep it out of the way of the menus.
    ui->centralWidget->layout()->addWidget(mUpdater); 
        
    mUpdater->checkForUpdates(silent); //check at startup is always silent.
}

void MainWindow::setApplicationTitle()
{
    QString cleanName = QFileInfo(mFile->fileName).baseName();
    if(cleanName.isEmpty())
        cleanName = "untitled";
    
    setWindowTitle(QString("%1 - %2[*]").arg(qApp->applicationName()).arg(cleanName));
}

void MainWindow::setupNewTabDialog()
{
    int rows = Settings::inst()->value("rowCount").toInt();
    int stitches = Settings::inst()->value("stitchCount").toInt();
    QString defSt = Settings::inst()->value("defaultStitch").toString();
    QString defStyle = Settings::inst()->value("chartStyle").toString();
    int incBy = Settings::inst()->value("increaseBy").toInt();
    
    ui->rows->setValue(rows);
    ui->stitches->setValue(stitches);
    ui->increaseBy->setValue(incBy);
    
    ui->defaultStitch->addItems(StitchLibrary::inst()->stitchList());
    ui->defaultStitch->setCurrentIndex(ui->defaultStitch->findText(defSt));

    ui->chartStyle->setCurrentIndex(ui->chartStyle->findText(defStyle));

    newChartUpdateStyle(defStyle);
    connect(ui->chartStyle, SIGNAL(currentIndexChanged(QString)), SLOT(newChartUpdateStyle(QString)));
    
    connect(ui->newDocBttnBox, SIGNAL(accepted()), this, SLOT(newChart()));
    connect(ui->newDocBttnBox, SIGNAL(rejected()), ui->newDocument, SLOT(hide()));   
}

void MainWindow::newChartUpdateStyle(QString style)
{

    if(style == tr("Blank")) {
        ui->rows->setEnabled(false);
        ui->stitches->setEnabled(false);
        ui->rowSpacing->setEnabled(false);
        ui->defaultStitch->setEnabled(false);
        ui->increaseBy->setEnabled(false);
    } else if(style == tr("Rounds")){
        ui->rows->setEnabled(true);
        ui->stitches->setEnabled(true);
        ui->rowSpacing->setEnabled(true);
        ui->defaultStitch->setEnabled(true);
        ui->rowsLbl->setText(tr("Rounds:"));
        ui->stitchesLbl->setText(tr("Starting Stitches:"));
        ui->increaseBy->setEnabled(true);
    } else if (style == tr("Rows")) {
        ui->rows->setEnabled(true);
        ui->stitches->setEnabled(true);
        ui->rowSpacing->setEnabled(true);
        ui->defaultStitch->setEnabled(true);
        ui->rowsLbl->setText(tr("Rows:"));
        ui->stitchesLbl->setText(tr("Stitches:"));
        ui->increaseBy->setEnabled(false);
    }
}

void MainWindow::setupStitchPalette()
{

    StitchSet* set = StitchLibrary::inst()->masterStitchSet();
    QSortFilterProxyModel* proxyModel = new QSortFilterProxyModel(this);

    proxyModel->setSourceModel(set);
    ui->allStitches->setModel(proxyModel);

    //TODO: setup a proxywidget that can hold header sections?
    StitchPaletteDelegate* delegate = new StitchPaletteDelegate(ui->allStitches);
    ui->allStitches->setItemDelegate(delegate);
    ui->allStitches->hideColumn(2);
    ui->allStitches->hideColumn(3);
    ui->allStitches->hideColumn(4);
    ui->allStitches->hideColumn(5);

    connect(ui->allStitches, SIGNAL(clicked(QModelIndex)), this, SLOT(selectStitch(QModelIndex)));
    connect(ui->patternStitches, SIGNAL(clicked(QModelIndex)), this, SLOT(selectStitch(QModelIndex)));
    connect(ui->patternColors, SIGNAL(clicked(QModelIndex)), this, SLOT(selectColor(QModelIndex)));
}

void MainWindow::setupDocks()
{
    //Undo Dock.
    mUndoDock = new QDockWidget(this);
    mUndoDock->setVisible(false);
    mUndoDock->setObjectName("undoHistory");
    QUndoView* view = new QUndoView(&mUndoGroup, mUndoDock);
    mUndoDock->setWidget(view);
    mUndoDock->setWindowTitle(tr("Undo History"));
    mUndoDock->setFloating(true);

    //Align & Distribute Dock
    mAlignDock = new AlignDock(this);
    connect(mAlignDock, SIGNAL(align(int)), SLOT(alignSelection(int)));
    connect(mAlignDock, SIGNAL(distribute(int)), SLOT(distributeSelection(int)));
    connect(mAlignDock, SIGNAL(visibilityChanged(bool)), ui->actionShowAlignDock, SLOT(setChecked(bool)));

    //Rows & Stitches Dock.
    mRowsDock = new RowsDock(this);
    mRowsDock->setEnabled(false); //TODO: remove this line when this is working again.
    connect(mRowsDock, SIGNAL(arrangeGrid(QSize,QSize,QSize,bool)), SLOT(arrangeGrid(QSize,QSize,QSize,bool)));
    connect(mRowsDock, SIGNAL(visibilityChanged(bool)), ui->actionShowRowsDock, SLOT(setChecked(bool)));
    
    //Mirror & Rotate.
    mMirrorDock = new MirrorDock(this);
    connect(mMirrorDock, SIGNAL(mirror(int)), SLOT(mirror(int)));
    connect(mMirrorDock, SIGNAL(rotate(qreal)), SLOT(rotate(qreal)));
    connect(mMirrorDock, SIGNAL(visibilityChanged(bool)), ui->actionShowMirrorDock, SLOT(setChecked(bool)));
}

void MainWindow::setupMenus()
{
    //File Menu
    connect(ui->menuFile, SIGNAL(aboutToShow()), SLOT(menuFileAboutToShow()));
    connect(ui->menuFile, SIGNAL(aboutToShow()), SLOT(menuRecentFilesAboutToShow()));
    connect(ui->actionNew, SIGNAL(triggered()), SLOT(fileNew()));
    connect(ui->actionOpen, SIGNAL(triggered()), SLOT(fileOpen()));
    connect(ui->actionSave, SIGNAL(triggered()), SLOT(fileSave()));
    connect(ui->actionSaveAs, SIGNAL(triggered()), SLOT(fileSaveAs()));

    connect(ui->actionPrint, SIGNAL(triggered()), SLOT(filePrint()));
    connect(ui->actionPrintPreview, SIGNAL(triggered()), SLOT(filePrintPreview()));
    connect(ui->actionExport, SIGNAL(triggered()), SLOT(fileExport()));

    connect(ui->actionQuit, SIGNAL(triggered()), SLOT(close()));
  
    ui->actionOpen->setIcon(QIcon::fromTheme("document-open", QIcon(":/images/fileopen.png")));
    ui->actionNew->setIcon(QIcon::fromTheme("document-new", QIcon(":/images/filenew.png")));
    ui->actionSave->setIcon(QIcon::fromTheme("document-save", QIcon(":/images/filesave.png")));
    ui->actionSaveAs->setIcon(QIcon::fromTheme("document-save-as", QIcon(":/images/filesave.png")));

    ui->actionPrint->setIcon(QIcon::fromTheme("document-print", QIcon(":/images/fileprint.png")));
    ui->actionPrintPreview->setIcon(QIcon::fromTheme("document-print-preview", QIcon(":/images/document-print-preview.png")));

    /*document-export*/
    ui->actionQuit->setIcon(QIcon::fromTheme("application-exit", QIcon(":/images/application-exit.png")));

    setupRecentFiles();
    
    //Edit Menu
    connect(ui->menuEdit, SIGNAL(aboutToShow()), SLOT(menuEditAboutToShow()));

    mActionUndo = mUndoGroup.createUndoAction(this, tr("Undo"));
    mActionRedo = mUndoGroup.createRedoAction(this, tr("Redo"));
    
    ui->menuEdit->insertAction(ui->actionCopy, mActionUndo);
    ui->menuEdit->insertAction(ui->actionCopy, mActionRedo);
    ui->menuEdit->insertSeparator(ui->actionCopy);

    ui->mainToolBar->insertAction(0, mActionUndo);
    ui->mainToolBar->insertAction(0, mActionRedo);
    ui->mainToolBar->insertSeparator(mActionUndo);
    
    mActionUndo->setIcon(QIcon::fromTheme("edit-undo", QIcon(":/images/editundo.png")));
    mActionRedo->setIcon(QIcon::fromTheme("edit-redo", QIcon(":/images/editredo.png")));
    mActionUndo->setShortcut(QKeySequence::Undo);
    mActionRedo->setShortcut(QKeySequence::Redo);
    
    ui->actionCopy->setIcon(QIcon::fromTheme("edit-copy", QIcon(":/images/editcopy.png")));
    ui->actionCut->setIcon(QIcon::fromTheme("edit-cut", QIcon(":/images/editcut.png")));
    ui->actionPaste->setIcon(QIcon::fromTheme("edit-paste", QIcon(":/images/editpaste.png")));

    connect(ui->actionCopy, SIGNAL(triggered()), SLOT(copy()));
    connect(ui->actionPaste, SIGNAL(triggered()), SLOT(paste()));
    connect(ui->actionCut, SIGNAL(triggered()), SLOT(cut()));

    connect(ui->actionColorSelectorBg, SIGNAL(triggered()), SLOT(selectColor()));
    //connect(ui->actionColorSelectorFg, SIGNAL(triggered()), this, SLOT(selectColor()));
    
    //ui->actionColorSelectorFg->setIcon(QIcon(drawColorBox(mFgColor, QSize(32, 32))));
    ui->actionColorSelectorBg->setIcon(QIcon(drawColorBox(mBgColor, QSize(32, 32))));
    
    //View Menu
    connect(ui->menuView, SIGNAL(aboutToShow()), SLOT(menuViewAboutToShow()));
    connect(ui->actionShowStitches, SIGNAL(triggered()), SLOT(viewShowStitches()));
    connect(ui->actionShowPatternColors, SIGNAL(triggered()), SLOT(viewShowPatternColors()));
    connect(ui->actionShowPatternStitches, SIGNAL(triggered()), SLOT(viewShowPatternStitches()));

    connect(ui->actionShowUndoHistory, SIGNAL(triggered()), SLOT(viewShowUndoHistory()));
    
    connect(ui->actionShowMainToolbar, SIGNAL(triggered()), SLOT(viewShowMainToolbar()));
    connect(ui->actionShowEditModeToolbar, SIGNAL(triggered()), SLOT(viewShowEditModeToolbar()));
    
    connect(ui->actionViewFullScreen, SIGNAL(triggered(bool)), SLOT(viewFullScreen(bool)));

    connect(ui->actionZoomIn, SIGNAL(triggered()), SLOT(viewZoomIn()));
    connect(ui->actionZoomOut, SIGNAL(triggered()), SLOT(viewZoomOut()));
    
    ui->actionZoomIn->setIcon(QIcon::fromTheme("zoom-in", QIcon(":/images/zoomin.png")));
    ui->actionZoomOut->setIcon(QIcon::fromTheme("zoom-out", QIcon(":/images/zoomout.png")));
    ui->actionZoomIn->setShortcut(QKeySequence::ZoomIn);
    ui->actionZoomOut->setShortcut(QKeySequence::ZoomOut);
    
    //Modes menu
    connect(ui->menuModes, SIGNAL(aboutToShow()), SLOT(menuModesAboutToShow()));

    mModeGroup = new QActionGroup(this);
    mModeGroup->addAction(ui->actionStitchMode);
    mModeGroup->addAction(ui->actionColorMode);
    mModeGroup->addAction(ui->actionAngleMode);
    mModeGroup->addAction(ui->actionStretchMode);
    mModeGroup->addAction(ui->actionCreateRows);
    mModeGroup->addAction(ui->actionIndicatorMode);

    connect(mModeGroup, SIGNAL(triggered(QAction*)), SLOT(changeTabMode(QAction*)));
    
    //Charts Menu
    connect(ui->actionAddChart, SIGNAL(triggered()), SLOT(documentNewChart()));
    connect(ui->actionRemoveTab, SIGNAL(triggered()), SLOT(removeCurrentTab()));

    connect(ui->actionShowChartCenter, SIGNAL(triggered()), SLOT(chartsShowChartCenter()));
    connect(ui->actionShowQuarterLines, SIGNAL(triggered()), SLOT(chartsShowQuarterLines()));
    
    ui->actionRemoveTab->setIcon(QIcon::fromTheme("tab-close", QIcon(":/images/tabclose.png")));
    
    connect(ui->menuChart, SIGNAL(aboutToShow()), SLOT(menuChartAboutToShow()));
    connect(ui->actionEditName, SIGNAL(triggered()), SLOT(chartEditName()));
    //TODO: get more icons from the theme for use with table editing.
    //http://doc.qt.nokia.com/4.7/qstyle.html#StandardPixmap-enum

    connect(ui->actionCreateRows, SIGNAL(toggled(bool)), SLOT(chartCreateRows(bool)));
    
    //Stitches Menu
    connect(ui->actionShowAlignDock, SIGNAL(triggered()), SLOT(viewShowAlignDock()));
    connect(ui->actionShowRowsDock, SIGNAL(triggered()), SLOT(viewShowRowsDock()));
    connect(ui->actionShowMirrorDock, SIGNAL(triggered()), SLOT(viewShowMirrorDock()));

    connect(ui->actionGroup, SIGNAL(triggered()), SLOT(group()));
    connect(ui->actionUngroup, SIGNAL(triggered()), SLOT(ungroup()));

    //Tools Menu
    connect(ui->menuTools, SIGNAL(aboutToShow()), SLOT(menuToolsAboutToShow()));
    connect(ui->actionOptions, SIGNAL(triggered()), SLOT(toolsOptions()));
    connect(ui->actionRegisterSoftware, SIGNAL(triggered()), SLOT(toolsRegisterSoftware()));
    connect(ui->actionStitchLibrary, SIGNAL(triggered()), SLOT(toolsStitchLibrary()));
    connect(ui->actionCheckForUpdates, SIGNAL(triggered()), SLOT(toolsCheckForUpdates()));
    
#ifdef APPLE_APP_STORE
    ui->actionCheckForUpdates->setVisible(false);
#endif

    //Help Menu
    connect(ui->actionAbout, SIGNAL(triggered()), SLOT(helpAbout()));
    connect(ui->actionCrochetHelp, SIGNAL(triggered()), SLOT(helpCrochetHelp()));
    
    //misc items
    connect(&mUndoGroup, SIGNAL(isModified(bool)), SLOT(documentIsModified(bool)));
    
    updateMenuItems();
}

void MainWindow::openRecentFile()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QAction* action = qobject_cast<QAction*>(sender());
    if (action && QFileInfo(action->data().toString()).exists()) {
        QStringList files;
        files.append(action->data().toString());
        loadFiles(files);
    }

    setApplicationTitle();
    updateMenuItems();
    QApplication::restoreOverrideCursor();
}

void MainWindow::addToRecentFiles(QString fileName)
{
    Settings::inst()->addRecentFile(fileName);
}

void MainWindow::menuRecentFilesAboutToShow()
{
    setupRecentFiles();
}

void MainWindow::setupRecentFiles()
{
    QStringList files;
    QStringList list = Settings::inst()->recentFiles();
    
    int maxRecentFiles = Settings::inst()->value("maxRecentFiles").toInt();
    mRecentFilesActs.clear();

    int i = 0;
    //remove any files that have been deleted or are not available.
    foreach(QString file, list) {
        if(QFileInfo(file).exists()) {
            files.append(file);
            i++;
            if(i >= maxRecentFiles)
                break;
        }
    }
    
    for(int i = 0; i < files.count(); ++i) {

        QString text = tr("&%1 %2").arg(i + 1).arg(QFileInfo(files[i]).fileName());
        QAction* a = new QAction(this);
        connect(a, SIGNAL(triggered()), SLOT(openRecentFile()));
        
        a->setText(text);
        a->setData(files[i]);
        if(i < maxRecentFiles)
            a->setVisible(true);
        else
            a->setVisible(false);
        mRecentFilesActs.append(a);
    }

    ui->menuOpenRecent->clear();
    ui->menuOpenRecent->addActions(mRecentFilesActs);

    //update the master list.
    Settings::inst()->setRecentFiles(files);
}

void MainWindow::updateMenuItems()
{
    menuFileAboutToShow();
    menuEditAboutToShow();
    menuViewAboutToShow();
    menuModesAboutToShow();
    menuChartAboutToShow();
}

void MainWindow::filePrint()
{
    if(Settings::inst()->isDemoVersion()) {
        Settings::inst()->trialVersionMessage(this);
        return;
    }
    
    //TODO: page count isn't working...
    QPrinter printer;
    QPrintDialog* dialog = new QPrintDialog(&printer, this);

    if(dialog->exec() != QDialog::Accepted)
        return;

    print(&printer);
}

void MainWindow::print(QPrinter* printer)
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    
    int tabCount = ui->tabWidget->count();
    QPainter* p = new QPainter();
    
    p->begin(printer);
    sws_debug(QString::number(tabCount));
    bool firstPass = true;
    for(int i = 0; i < tabCount; ++i) {
        if(!firstPass)
            printer->newPage();
        
        CrochetTab* tab = qobject_cast<CrochetTab*>(ui->tabWidget->widget(i));
        tab->renderChart(p);
        firstPass = false;
    }
    p->end();
    
    QApplication::restoreOverrideCursor();
}

void MainWindow::filePrintPreview()
{
    if(Settings::inst()->isDemoVersion()) {
        Settings::inst()->trialVersionMessage(this);
        return;
    }
    
    //FIXME: this isn't working
    QPrinter* printer = new QPrinter(QPrinter::HighResolution);
    QPrintPreviewDialog* dialog = new QPrintPreviewDialog(printer, this);
    connect(dialog, SIGNAL(paintRequested(QPrinter*)), this, SLOT(print(QPrinter*)));
    
    dialog->exec();
}

void MainWindow::fileExport()
{
    if(!hasTab())
        return;
    
    ExportUi d(ui->tabWidget, &mPatternStitches, &mPatternColors, this);
    d.exec();
}

QPixmap MainWindow::drawColorBox(QColor color, QSize size)
{
    QPixmap pix = QPixmap(size);
    QPainter p;
    p.begin(&pix);
    p.fillRect(QRect(QPoint(0, 0), size), QColor(color));
    p.drawRect(0, 0, size.width() - 1, size.height() - 1);
    p.end();

    return pix;
}

void MainWindow::selectColor()
{
    if(sender() == ui->actionColorSelectorBg) {
        QColor color = QColorDialog::getColor(mBgColor, this, tr("Background Color"));
        
        if (color.isValid()) {
            ui->actionColorSelectorBg->setIcon(QIcon(drawColorBox(QColor(color), QSize(32, 32))));
            mBgColor = color;
            updateBgColor();
        }
    } else {
        QColor color = QColorDialog::getColor(mFgColor, this, tr("Foreground Color"));
        
        if (color.isValid()) {
            //ui->actionColorSelectorFg->setIcon(QIcon(drawColorBox(QColor(color), QSize(32, 32))));
            mFgColor = color;
            updateFgColor();
        }
    }
}

void MainWindow::updateBgColor()
{
    for(int i = 0; i < ui->tabWidget->count(); ++i) {
        CrochetTab* tab = qobject_cast<CrochetTab*>(ui->tabWidget->widget(i));
        if(tab)
            tab->setEditBgColor(mBgColor);
    }
    setEditMode(11);
}

void MainWindow::updateFgColor()
{
    for(int i = 0; i < ui->tabWidget->count(); ++i) {
        CrochetTab* tab = qobject_cast<CrochetTab*>(ui->tabWidget->widget(i));
        if(tab)
            tab->setEditFgColor(mFgColor);
    }
    setEditMode(11);
}

void MainWindow::selectStitch(QModelIndex index)
{
    QModelIndex idx;
    
    if(sender() == ui->allStitches) {
        const QSortFilterProxyModel* model =  static_cast<const QSortFilterProxyModel*>(index.model());
        idx = model->mapToSource(model->index(index.row(), 0));
        
    } else
        idx = index;
    
    QString stitch = idx.data(Qt::DisplayRole).toString();

    if(stitch.isEmpty())
        return;
    
    for(int i = 0; i < ui->tabWidget->count(); ++i) {
        CrochetTab* tab = qobject_cast<CrochetTab*>(ui->tabWidget->widget(i));
        if(tab)
            tab->setEditStitch(stitch);
    }
    setEditMode(10);
}

void MainWindow::selectColor(QModelIndex index)
{
    QString color = index.data(Qt::ToolTipRole).toString();

    for(int i = 0; i < ui->tabWidget->count(); ++i) {
        CrochetTab* tab = qobject_cast<CrochetTab*>(ui->tabWidget->widget(i));
        if(tab)
            tab->setEditBgColor(color);
    }
    setEditMode(11);
}

void MainWindow::documentNewChart()
{
    int rows = Settings::inst()->value("rowCount").toInt();
    int stitches = Settings::inst()->value("stitchCount").toInt();
    int incBy = Settings::inst()->value("increaseBy").toInt();
    
    ui->rows->setValue(rows);
    ui->stitches->setValue(stitches);
    ui->increaseBy->setValue(incBy);
    
    ui->chartTitle->setText(nextChartName());

    if(ui->newDocument->isVisible()) {
        QPalette pal = ui->newDocument->palette();
        mNewDocWidgetColor = ui->newDocument->palette().color(ui->newDocument->backgroundRole());
        pal.setColor(ui->newDocument->backgroundRole(), ui->newDocument->palette().highlight().color());
        ui->newDocument->setPalette(pal);
        QTimer::singleShot(1500, this, SLOT(flashNewDocDialog()));
    } else 
        ui->newDocument->show();
}

void MainWindow::helpCrochetHelp()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QString path = QApplication::applicationDirPath();
    QString file ="";
#ifdef Q_WS_WIN
    file= QString("file://%1/Crochet_Charts_User_Guide_%2.pdf").arg(path).arg(AppInfo::inst()->appVersionShort);
    bool r = QDesktopServices::openUrl(QUrl::fromLocalFile(file));
#endif

#ifdef Q_WS_MAC
    file = QString("file://%1/Crochet_Charts_User_Guide_%2.pdf").arg(path).arg(AppInfo::inst()->appVersionShort);
    QDesktopServices::openUrl(QUrl::fromLocalFile(file));
#endif

#ifdef Q_WS_X11
    file = QString("file://%1/../share/Crochet_Charts/Crochet_Charts_User_Guide_%2.pdf").arg(path).arg(AppInfo::inst()->appVersionShort);
    QDesktopServices::openUrl(QUrl::fromLocalFile(file));
#endif //Q_WS_WIN

    QApplication::restoreOverrideCursor();

}

void MainWindow::helpAbout()
{
    QString aboutInfo = QString(tr("<h1>%1</h1>"
                                   "<p>Version: %2 (built on %3)</p>"
                                   "<p>Copyright (c) 2010-2011 %4</p>"
                                   "<p>This software is for creating crochet charts that"
                                   " can be exported in many differnet file types.</p>")
                                .arg(qApp->applicationName())
                                .arg(qApp->applicationVersion())
                                .arg(AppInfo::inst()->appBuildInfo)
                                .arg(qApp->organizationName())
                                );
    QString fName = Settings::inst()->value("firstName").toString();
    QString lName = Settings::inst()->value("lastName").toString();
    QString email = Settings::inst()->value("email").toString();
    QString sn = Settings::inst()->value("serialNumber").toString();

    QString dedication = tr("<p></p>");
    aboutInfo.append(dedication);
    
    QString licenseInfo;

#ifndef APPLE_APP_STORE
    if(Settings::inst()->isDemoVersion()) {
        licenseInfo = QString(tr("<p>This is a demo license granted to:<br />"
                              "Name: %1 %2<br />"
                              "Email: %3<br /></p>")
                              .arg(fName).arg(lName).arg(email));
    } else {
        licenseInfo = QString(tr("<p>This software is licensed to:<br />"
                              "Name: %1 %2<br />"
                              "Email: %3<br />"
                              "Serial #: %4</p>")
                              .arg(fName).arg(lName).arg(email).arg(sn));
    }
#else
    licenseInfo = QString(tr("<p>This version was downloaded from the Apple App Store</p>"));
#endif

    aboutInfo.append(licenseInfo);
    QMessageBox::about(this, tr("About Crochet Charts"), aboutInfo);
}

void MainWindow::closeEvent(QCloseEvent* event)
{

    if(safeToClose()) {
        Settings::inst()->setValue("geometry", saveGeometry());
        Settings::inst()->setValue("windowState", saveState());

        if(Settings::inst()->files.contains(mFile->fileName.toLower()))
            Settings::inst()->files.remove(mFile->fileName.toLower());

        mFile->cleanUp();

        QMainWindow::closeEvent(event);
    } else {
        event->ignore();
    }
    
}

bool MainWindow::safeToClose()
{
    //only prompt to save the file if it has tabs.
    if(ui->tabWidget->count() > 0) {
        if(isWindowModified())
            return promptToSave();
    }
    
    return true;
}

bool MainWindow::promptToSave()
{
    QString niceName = QFileInfo(mFile->fileName).baseName();
    if(niceName.isEmpty())
        niceName = "untitled";
    
    QMessageBox msgbox(this);
    msgbox.setText(tr("The document '%1' has unsaved changes.").arg(niceName));
    msgbox.setInformativeText(tr("Do you want to save the changes?"));
    msgbox.setIcon(QMessageBox::Warning);
    msgbox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    
    int results = msgbox.exec();
    
    if(results == QMessageBox::Cancel)
        return false;
    else if(results == QMessageBox::Discard)
        return true;
    else if (results == QMessageBox::Save) {
        //FIXME: if the user cancels the fileSave() we should drop them back to the window not close it.
        fileSave();
        return true;
    }

    return false;
}

void MainWindow::readSettings()
{
    //TODO: For full session restoration reimplement QApplication::commitData()
    //See: http://doc.qt.nokia.com/stable/session.html
    restoreGeometry(Settings::inst()->value("geometry").toByteArray());
    restoreState(Settings::inst()->value("windowState").toByteArray());

}

void MainWindow::menuToolsAboutToShow()
{
    if(!Settings::inst()->isDemoVersion())
        ui->actionRegisterSoftware->setVisible(false);    
}

void MainWindow::toolsOptions()
{
    SettingsUi dialog(this);
    dialog.exec();
    if(curCrochetTab())
        curCrochetTab()->sceneUpdate();
}

void MainWindow::fileOpen()
{
    QString fileLoc = Settings::inst()->value("fileLocation").toString();
    QString fileName = QFileDialog::getOpenFileName(this,
         tr("Open Crochet Pattern"), fileLoc, tr("Crochet Pattern (*.pattern)"));

    if(fileName.isEmpty() || fileName.isNull())
        return;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    if(!Settings::inst()->files.contains(fileName.toLower())) {
        if(ui->tabWidget->count() > 0) {
            QStringList files;
            files.append(fileName);
            MainWindow* newWin = new MainWindow(files);
            newWin->move(x() + 40, y() + 40);
            newWin->show();
            Settings::inst()->files.insert(fileName.toLower(), newWin);
        } else {
            ui->newDocument->hide();
            mFile->fileName = fileName;
            mFile->load();
            Settings::inst()->files.insert(mFile->fileName.toLower(), this);
        }
        
        addToRecentFiles(fileName);

        setApplicationTitle();
        updateMenuItems();
    } else {
        //show the window if it's already open.
        MainWindow* win = Settings::inst()->files.find(fileName.toLower()).value();
        win->raise();
    }
    QApplication::restoreOverrideCursor();
}

void MainWindow::fileSave()
{
    if(Settings::inst()->isDemoVersion()) {
        Settings::inst()->trialVersionMessage(this);
        return;
    }

    if(ui->tabWidget->count() <= 0) {
        QMessageBox msgbox;
        msgbox.setText(tr("%1 cannot save a document without at least one (1) chart.").arg(qAppName()));
        msgbox.exec();
        return;
    }
    
    if(mFile->fileName.isEmpty())
        fileSaveAs();
    else {
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        SaveFile::FileError err = mFile->save();
        if(err != SaveFile::No_Error)
            qWarning() << "There was an error saving the file: " << err;
        QApplication::restoreOverrideCursor();
    }
    
    setWindowModified(false);
}

void MainWindow::fileSaveAs()
{
    if(Settings::inst()->isDemoVersion()) {
        Settings::inst()->trialVersionMessage(this);
        return;
    }

    QString fileLoc = Settings::inst()->value("fileLocation").toString();
    QString fileName = QFileDialog::getSaveFileName(this,
            tr("Save Crochet Pattern"), fileLoc, tr("Crochet Pattern (*.pattern)"));

    if(fileName.isEmpty())
        return;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    //update the list of open files.
    if(Settings::inst()->files.contains(mFile->fileName.toLower()))
        Settings::inst()->files.remove(mFile->fileName.toLower());
    Settings::inst()->files.insert(fileName.toLower(), this);

    mFile->fileName = fileName;
    mFile->save();

    setApplicationTitle();
    setWindowModified(false);
    QApplication::restoreOverrideCursor();
}

void MainWindow::viewFullScreen(bool state)
{
    if(state)
        showFullScreen();
    else
        showNormal();
}

void MainWindow::menuFileAboutToShow()
{
    bool state = hasTab();

    ui->actionSave->setEnabled(state);
    ui->actionSaveAs->setEnabled(state);

    ui->actionPrint->setEnabled(state);
    ui->actionPrintPreview->setEnabled(state);
    
    ui->actionExport->setEnabled(state);

    //ui->actionColorSelectorFg->setEnabled(state);
    ui->actionColorSelectorBg->setEnabled(state);
}

void MainWindow::menuEditAboutToShow()
{
    bool state = hasTab();
    
    ui->actionCopy->setEnabled(state);
    ui->actionCut->setEnabled(state);
    ui->actionPaste->setEnabled(state);
    
}

void MainWindow::menuViewAboutToShow()
{
    ui->actionShowStitches->setChecked(ui->allStitchesDock->isVisible());
    ui->actionShowPatternColors->setChecked(ui->patternColorsDock->isVisible());
    ui->actionShowPatternStitches->setChecked(ui->patternStitchesDock->isVisible());

    ui->actionShowUndoHistory->setChecked(mUndoDock->isVisible());
    
    ui->actionShowEditModeToolbar->setChecked(ui->editModeToolBar->isVisible());
    ui->actionShowMainToolbar->setChecked(ui->mainToolBar->isVisible());
    
    ui->actionViewFullScreen->setChecked(isFullScreen());

    bool state = hasTab();
    ui->actionZoomIn->setEnabled(state);
    ui->actionZoomOut->setEnabled(state);

    ui->actionShowAlignDock->setChecked(mAlignDock->isVisible());
    ui->actionShowRowsDock->setChecked(mRowsDock->isVisible());
    ui->actionShowMirrorDock->setChecked(mMirrorDock->isVisible());
}

void MainWindow::fileNew()
{

    if(ui->tabWidget->count() > 0) {
        MainWindow* newWin = new MainWindow;
        newWin->move(x() + 40, y() + 40);
        newWin->show();
        newWin->ui->newDocument->show();
    } else {
        if(ui->newDocument->isVisible()) {
            QPalette pal = ui->newDocument->palette();
            mNewDocWidgetColor = ui->newDocument->palette().color(ui->newDocument->backgroundRole());
            pal.setColor(ui->newDocument->backgroundRole(), ui->newDocument->palette().highlight().color());
            ui->newDocument->setPalette(pal);
            QTimer::singleShot(1500, this, SLOT(flashNewDocDialog()));
        } else
            ui->newDocument->show();
    }
    
}

void MainWindow::flashNewDocDialog()
{
    QPalette pal = ui->newDocument->palette();
    pal.setColor(ui->newDocument->backgroundRole(), mNewDocWidgetColor);
    ui->newDocument->setPalette(pal);
}

void MainWindow::newChart()
{
    ui->newDocument->hide();

    int rows = ui->rows->text().toInt();
    int cols = ui->stitches->text().toInt();
    QString defStitch = ui->defaultStitch->currentText();
    QString name = ui->chartTitle->text();
    int incBy = ui->increaseBy->text().toInt();
    
    QString style = ui->chartStyle->currentText();

    Scene::ChartStyle st = Scene::Rows;

    if(style == tr("Blank"))
        st = Scene::Blank;
    else if(style == tr("Rounds"))
        st = Scene::Rounds;
    else
        st = Scene::Rows;

    if(docHasChartName(name))
        name = nextChartName(name);

    CrochetTab* tab = createTab(st);

    if(name.isEmpty())
            name = nextChartName();

    ui->tabWidget->addTab(tab, name);
    ui->tabWidget->setCurrentWidget(tab);

    QString ddValue = ui->rowSpacing->currentText();
    qreal rowHeight = 96;

    if(ddValue == tr("1 Chain"))
        rowHeight = 32;
    else if (ddValue == tr("2 Chains"))
        rowHeight = 64;
    else if (ddValue == tr("3 Chains"))
        rowHeight = 96;
    else if (ddValue == tr("4 Chains"))
        rowHeight = 128;
    else if (ddValue == tr("5 Chains"))
        rowHeight = 160;
    else if (ddValue == tr("6 Chains"))
        rowHeight = 182;

    tab->createChart(st, rows, cols, defStitch, QSizeF(32, rowHeight), incBy);

    updateMenuItems();
    documentIsModified(true);
}

CrochetTab* MainWindow::createTab(Scene::ChartStyle style)
{

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    CrochetTab* tab = new CrochetTab(style, mEditMode, mStitch, mFgColor, mBgColor, ui->tabWidget);
    tab->setPatternStitches(&mPatternStitches);
    tab->setPatternColors(&mPatternColors);

    connect(tab, SIGNAL(chartStitchChanged()), SLOT(updatePatternStitches()));
    connect(tab, SIGNAL(chartColorChanged()), SLOT(updatePatternColors()));
    connect(tab, SIGNAL(tabModified(bool)), SLOT(documentIsModified(bool)));

    mUndoGroup.addStack(tab->undoStack());
    
    QApplication::restoreOverrideCursor();

    return tab;
}

QString MainWindow::nextChartName(QString baseName)
{
    QString nextName = baseName;

    int i = 1;
    
    while(docHasChartName(nextName)) {
        nextName = baseName + QString::number(i);
        i++;
    }
    
    return nextName;
}

bool MainWindow::docHasChartName(QString name)
{
    int tabCount = ui->tabWidget->count();
    for(int i = 0; i < tabCount; ++i) {
        if (ui->tabWidget->tabText(i) == name)
            return true;
    }

    return false;
}

void MainWindow::viewShowStitches()
{
    ui->allStitchesDock->setVisible(ui->actionShowStitches->isChecked());
}

void MainWindow::viewShowPatternColors()
{
    ui->patternColorsDock->setVisible(ui->actionShowPatternColors->isChecked());
}

void MainWindow::viewShowPatternStitches()
{
    ui->patternStitchesDock->setVisible(ui->actionShowPatternStitches->isChecked());
}

void MainWindow::viewShowUndoHistory()
{
    mUndoDock->setVisible(ui->actionShowUndoHistory->isChecked());
}

void MainWindow::viewShowEditModeToolbar()
{
    ui->editModeToolBar->setVisible(ui->actionShowEditModeToolbar->isChecked());
}

void MainWindow::viewShowMainToolbar()
{
    ui->mainToolBar->setVisible(ui->actionShowMainToolbar->isChecked());
}

void MainWindow::viewShowAlignDock()
{
    mAlignDock->setVisible(ui->actionShowAlignDock->isChecked());
}

void MainWindow::viewShowRowsDock()
{
    mRowsDock->setVisible(ui->actionShowRowsDock->isChecked());
}

void MainWindow::viewShowMirrorDock()
{
    mMirrorDock->setVisible(ui->actionShowMirrorDock->isChecked());
}

void MainWindow::menuModesAboutToShow()
{
    bool enabled = false;
    bool selected = false;
    bool checkedItem = false;
    bool used = false;
    
    QStringList modes;
    if(hasTab()) {
        CrochetTab* tab = qobject_cast<CrochetTab*>(ui->tabWidget->currentWidget());
        modes = tab->editModes();
    }

    foreach(QAction* a, mModeGroup->actions()) {
        if(modes.contains(a->text()))
            enabled = true;
        else
            enabled = false;

        if(mModeGroup->checkedAction() && mModeGroup->checkedAction() == a)
            checkedItem = true;
        
        if(enabled && !used && (!mModeGroup->checkedAction() || checkedItem)) {
            selected = true;
            used = true;
        }
        
        a->setEnabled(enabled);
        a->setChecked(selected);
        selected = false;
    }   
}

void MainWindow::changeTabMode(QAction* a)
{
    int mode = -1;
    
    if(a == ui->actionStitchMode)
        mode = 10;
    else if(a == ui->actionColorMode)
        mode = 11;
    else if(a == ui->actionCreateRows)
        mode = 12;
    else if(a == ui->actionAngleMode)
        mode = 14;
    else if(a == ui->actionStretchMode)
        mode = 15;
    else if(a == ui->actionIndicatorMode)
        mode = 16;
    
    setEditMode(mode);
}

void MainWindow::setEditMode(int mode)
{
    mEditMode = mode;
    
    if(mode == 10)
        ui->actionStitchMode->setChecked(true);
    else if(mode == 11)
        ui->actionColorMode->setChecked(true);
    else if(mode == 12)
        ui->actionCreateRows->setChecked(true);
    else if(mode == 14)
        ui->actionAngleMode->setChecked(true);
    else if(mode == 15)
        ui->actionStretchMode->setChecked(true);
    else if(mode == 16)
        ui->actionIndicatorMode->setChecked(true);
    
    for(int i = 0; i < ui->tabWidget->count(); ++i) {
        CrochetTab* tab = qobject_cast<CrochetTab*>(ui->tabWidget->widget(i));
        if(tab)
            tab->setEditMode(mEditMode);
    }
}

void MainWindow::menuChartAboutToShow()
{
    bool state = hasTab();
    ui->actionRemoveTab->setEnabled(state);
    ui->actionEditName->setEnabled(state);
    ui->actionShowChartCenter->setEnabled(state);
    ui->actionShowQuarterLines->setEnabled(state);

    CrochetTab* tab = curCrochetTab();
    if(tab) {
        ui->actionShowChartCenter->setChecked(tab->hasChartCenter());
        ui->actionShowQuarterLines->setChecked(tab->hasQuarterLines());
    } else {
        ui->actionShowChartCenter->setChecked(false);
        ui->actionShowQuarterLines->setChecked(false);
    }

}

void MainWindow::chartsShowChartCenter()
{

    CrochetTab* tab = curCrochetTab();
    if(tab) {
        tab->setChartCenter(ui->actionShowChartCenter->isChecked());
    }

}

void MainWindow::chartsShowQuarterLines()
{
    CrochetTab* tab = curCrochetTab();
    if(tab) {
        tab->setQuarterLines(ui->actionShowQuarterLines->isChecked());
    }
}

void MainWindow::chartEditName()
{
    if(!ui->tabWidget->currentWidget())
        return;
    
    int curTab = ui->tabWidget->currentIndex();
    QString currentName  = ui->tabWidget->tabText(curTab);
    bool ok;
    QString newName = QInputDialog::getText(this, tr("Set Chart Name"), tr("Chart name:"),
                                            QLineEdit::Normal, currentName, &ok);
    if(ok && !newName.isEmpty()) {
        ui->tabWidget->setTabText(curTab, newName);
        if(newName != currentName)
            documentIsModified(true);
    }
}

void MainWindow::toolsRegisterSoftware()
{
    if(Settings::inst()->isDemoVersion()) {
        LicenseWizard wizard(true, this);
        if(wizard.exec() == QWizard::Accepted) {
                Settings::inst()->setDemoVersion(false);
                return;
        }
    }
}

void MainWindow::toolsCheckForUpdates()
{
    bool silent = false;
    checkUpdates(silent);
}

void MainWindow::toolsStitchLibrary()
{
    StitchLibraryUi d(this);
    d.exec();
    
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    StitchLibrary::inst()->saveAllSets();
    QApplication::restoreOverrideCursor();

    StitchLibrary::inst()->reloadAllStitchIcons();
}

void MainWindow::viewZoomIn()
{
    CrochetTab* tab = curCrochetTab();
    if(!tab)
        return;
    tab->zoomIn();
}

void MainWindow::viewZoomOut()
{
    CrochetTab* tab = curCrochetTab();
    if(!tab)
        return;
    tab->zoomOut();
}

CrochetTab* MainWindow::curCrochetTab()
{
    CrochetTab* tab = qobject_cast<CrochetTab*>(ui->tabWidget->currentWidget());
    return tab;
}

bool MainWindow::hasTab()
{
    CrochetTab* cTab = qobject_cast<CrochetTab*>(ui->tabWidget->currentWidget());
    if(!cTab)
        return false;

    return true;
}

QTabWidget* MainWindow::tabWidget()
{
    return ui->tabWidget;
}

void MainWindow::tabChanged(int newTab)
{
    if(newTab == -1)
        return;

    CrochetTab* tab = qobject_cast<CrochetTab*>(ui->tabWidget->widget(newTab));
    if(!tab)
        return;
    
    mUndoGroup.setActiveStack(tab->undoStack());
}

void MainWindow::removeCurrentTab()
{
    removeTab(ui->tabWidget->currentIndex());
}

void MainWindow::removeTab(int tabIndex)
{
    if(tabIndex < 0)
        return;

    QMessageBox msgbox;
    
    if(ui->tabWidget->count() == 1) {
        msgbox.setText(tr("A document must have at least one (1) chart."));
        msgbox.setIcon(QMessageBox::Information);
        msgbox.exec();
        return;
    }
    
    msgbox.setWindowTitle(tr("Delete Chart"));
    msgbox.setText(tr("Are you sure you want to delete this chart from the document?"));
    msgbox.setInformativeText(tr("Deleting a chart from the document is a permanent procedure."));
    msgbox.setIcon(QMessageBox::Question);
    /*QPushButton* removeChart =*/ msgbox.addButton(tr("Delete the chart"), QMessageBox::AcceptRole);
    QPushButton* keepChart = msgbox.addButton(tr("Keep the chart"), QMessageBox::RejectRole);

    msgbox.exec();
    
    if(msgbox.clickedButton() == keepChart)
        return;
    
    //FIXME: Make removing a tab undo-able, using a *tab and chart name.
    ui->tabWidget->removeTab(tabIndex);

    documentIsModified(true);
    
    //update the title and menus
    setApplicationTitle();
    updateMenuItems();
}

void MainWindow::updatePatternStitches()
{
    if(ui->tabWidget->count() <= 0)
        return;

    //FIXME: this whole thing needs to be worked out, but the very least is make this use a shared icon.
    ui->patternStitches->clear();
    QMapIterator<QString, int> i(mPatternStitches);
    while (i.hasNext()) {
        i.next();
        QList<QListWidgetItem*> items = ui->patternStitches->findItems(i.key(), Qt::MatchExactly);
        if(items.count() == 0) {
            Stitch* s = StitchLibrary::inst()->findStitch(i.key());
            QPixmap pix = QPixmap(QSize(32, 32));
            pix.load(s->file());
            QIcon icon = QIcon(pix);
            QListWidgetItem* item = new QListWidgetItem(icon, i.key(), ui->patternStitches);
            ui->patternStitches->addItem(item);
        }
    }
}

void MainWindow::updatePatternColors()
{
    if(ui->tabWidget->count() <= 0)
        return;

    ui->patternColors->clear();

    QString prefix = Settings::inst()->value("colorPrefix").toString();

    QStringList keys = mPatternColors.keys();
    QMap<qint64, QString> sortedColors;
    
    foreach(QString key, keys) {
        qint64 added = mPatternColors.value(key).value("added");
        sortedColors.insert(added, key);
    }

    int i = 1;
    QList<qint64> sortedKeys = sortedColors.keys();
    foreach(qint64 sortedKey, sortedKeys) {
        QString color = sortedColors.value(sortedKey);
        QList<QListWidgetItem*> items = ui->patternColors->findItems(color, Qt::MatchExactly);
        if(items.count() == 0) {
            QPixmap pix = drawColorBox(color, QSize(32, 32));
            QIcon icon = QIcon(pix);
            
            QListWidgetItem* item = new QListWidgetItem(icon, prefix + QString::number(i), ui->patternColors);
            item->setToolTip(color);
            ui->patternColors->addItem(item);
            ++i;
        }
    }
}

void MainWindow::documentIsModified(bool isModified)
{
    //TODO: check all possible modification locations.
    setWindowModified(isModified);
}

void MainWindow::chartCreateRows(bool state)
{
    CrochetTab* tab = curCrochetTab();
    if(tab) tab->showRowEditor(state);
}

void MainWindow::alignSelection(int style)
{
    CrochetTab* tab = curCrochetTab();
    if(tab) tab->alignSelection(style);
}

void MainWindow::distributeSelection(int style)
{
    CrochetTab* tab = curCrochetTab();
    if(tab) tab->distributeSelection(style);
}

void MainWindow::arrangeGrid(QSize grid, QSize alignment, QSize spacing, bool useSelection)
{
    CrochetTab* tab = curCrochetTab();
    if(tab) tab->arrangeGrid(grid, alignment, spacing, useSelection);
}

void MainWindow::mirror(int direction)
{
    CrochetTab* tab = curCrochetTab();
    if(tab) tab->mirror(direction);
}

void MainWindow::rotate(qreal degrees)
{
    CrochetTab* tab = curCrochetTab();
    if(tab) tab->rotate(degrees);
}

void MainWindow::copy()
{
    CrochetTab* tab = curCrochetTab();
    if(tab) tab->copy();
}

void MainWindow::cut()
{
    CrochetTab* tab = curCrochetTab();
    if(tab) tab->cut();
}

void MainWindow::paste()
{
    CrochetTab* tab = curCrochetTab();
    if(tab) tab->paste();
}

void MainWindow::group()
{
    CrochetTab* tab = curCrochetTab();
    if(tab) tab->group();
}

void MainWindow::ungroup()
{
    CrochetTab* tab = curCrochetTab();
    if(tab) tab->ungroup();

}
