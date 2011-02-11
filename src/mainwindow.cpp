/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDialog>
#include <QMessageBox>
#include <QFileDialog>

#include "savefile.h"

#include "appinfo.h"
#include "charttab.h"

#include "settings.h"
#include "settingsui.h"

#include "stitchcollection.h"

#include "licensewizard.h"
#include "exportui.h"
#include "stitchlibraryui.h"

#include "stitchpalettedelegate.h"
#include "stitchset.h"

#include <QDebug>
#include "crochetscene.h"
#include <QInputDialog>

#include <QPrinter>
#include <QtSvg/QSvgGenerator>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QSvgRenderer>

MainWindow::MainWindow(QWidget *parent, QString fileName)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    ui->setupUi(this);

    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));
    connect(ui->tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(removeTab(int)));
    
    setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

    checkUpdates();
    
    setupStitchPalette();
    readSettings();

    mFile = new SaveFile(this);
    if(!fileName.isEmpty()) {
        mFile->fileName = fileName;
        mFile->load();
    }

    setupNewTabDialog();

    setupMenus();
    QApplication::restoreOverrideCursor();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete StitchCollection::inst();
}

void MainWindow::checkUpdates()
{
    //TODO: check for updates in a seperate thread.
    mUpdater = new Updater(this);
    // append the updater to the centralWidget to keep it out of the way of the menus.
    ui->centralWidget->layout()->addWidget(mUpdater); 
        
    bool checkForUpdates = Settings::inst()->value("checkForUpdates").toBool();
    if(checkForUpdates)
        mUpdater->checkForUpdates(true); //check at startup is always silent.
}

void MainWindow::setupNewTabDialog()
{
    int rows = Settings::inst()->value("defaultRows").toInt();
    int stitches = Settings::inst()->value("defaultStitches").toInt();
    
    ui->rows->setValue(rows);
    ui->stitches->setValue(stitches);
    
    //TODO: see if you can make "returnPressed" work for the spinboxes.
    connect(ui->chartTitle, SIGNAL(returnPressed()), this, SLOT(newChart()));
    
    connect(ui->newDocBttnBox, SIGNAL(accepted()), this, SLOT(newChart()));
    connect(ui->newDocBttnBox, SIGNAL(rejected()), ui->newDocument, SLOT(hide()));   
}

void MainWindow::setupStitchPalette()
{
    StitchSet *set = StitchCollection::inst()->masterStitchSet();
    ui->allStitches->setModel(set);

    //TODO: setup a proxywidget that can hold header sections?
    //StitchPaletteDelegate *delegate = new StitchPaletteDelegate(ui->allStitches);
    //ui->allStitches->setItemDelegate(delegate);
}

void MainWindow::setupMenus()
{
    //File Menu
    connect(ui->menuFile, SIGNAL(aboutToShow()), this, SLOT(menuFileAboutToShow()));
    connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(fileNew()));
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(fileOpen()));
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(fileSave()));
    connect(ui->actionSaveAs, SIGNAL(triggered()), this, SLOT(fileSaveAs()));

    connect(ui->actionPrint, SIGNAL(triggered()), this, SLOT(filePrint()));
    connect(ui->actionPrintPreview, SIGNAL(triggered()), this, SLOT(filePrintPreview()));
    connect(ui->actionExport, SIGNAL(triggered()), this, SLOT(fileExport()));

    connect(ui->actionClose, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionQuit, SIGNAL(triggered()), qApp, SLOT(quit()));
    
    
    ui->actionOpen->setIcon(QIcon::fromTheme("document-open" /*, QIcon(":/file-open.png")*/));
    ui->actionNew->setIcon(QIcon::fromTheme("document-new"));
    ui->actionSave->setIcon(QIcon::fromTheme("document-save"));
    ui->actionSaveAs->setIcon(QIcon::fromTheme("document-save-as"));

    ui->actionPrint->setIcon(QIcon::fromTheme("document-print"));
    ui->actionPrintPreview->setIcon(QIcon::fromTheme("file-print"));
    ui->actionExport->setIcon(QIcon::fromTheme("export-pdf"));
    
    ui->actionClose->setIcon(QIcon::fromTheme("document-close"));

    //Edit Menu
    connect(ui->menuEdit, SIGNAL(aboutToShow()), this, SLOT(menuEditAboutToShow()));

    mActionUndo = mUndoGroup.createUndoAction(this, tr("Undo"));
    mActionRedo = mUndoGroup.createRedoAction(this, tr("Redo"));
    
    ui->menuEdit->insertAction(ui->actionCopy, mActionUndo);
    ui->menuEdit->insertAction(ui->actionCopy, mActionRedo);
    ui->menuEdit->insertSeparator(ui->actionCopy);
    
    mActionUndo->setIcon(QIcon::fromTheme("edit-undo"));
    mActionRedo->setIcon(QIcon::fromTheme("edit-redo"));
    mActionUndo->setShortcut(QKeySequence::Undo);
    mActionRedo->setShortcut(QKeySequence::Redo);
    
    ui->actionCopy->setIcon(QIcon::fromTheme("edit-copy" /*, QIcon(":/edit-copy.png")*/));
    ui->actionCut->setIcon(QIcon::fromTheme("edit-cut" /*, QIcon(":/edit-cut.png")*/));
    ui->actionPaste->setIcon(QIcon::fromTheme("edit-paste" /*, QIcon(":/edit-paste.png")*/));

    //View Menu
    connect(ui->menuView, SIGNAL(aboutToShow()), this, SLOT(menuViewAboutToShow()));
    connect(ui->actionShowStitches, SIGNAL(triggered()), this, SLOT(viewShowStitches()));
    connect(ui->actionShowPatternColors, SIGNAL(triggered()), this, SLOT(viewShowPatternColors()));
    connect(ui->actionShowPatternStitches, SIGNAL(triggered()), this, SLOT(viewShowPatternStitches()));
    
    connect(ui->actionViewFullScreen, SIGNAL(triggered(bool)), this, SLOT(viewFullScreen(bool)));

    connect(ui->actionZoomIn, SIGNAL(triggered(bool)), this, SLOT(viewZoomIn()));
    connect(ui->actionZoomOut, SIGNAL(triggered(bool)), this, SLOT(viewZoomOut()));
    
    ui->actionZoomIn->setIcon(QIcon::fromTheme("zoom-in"));
    ui->actionZoomOut->setIcon(QIcon::fromTheme("zoom-out"));
    ui->actionZoomIn->setShortcut(QKeySequence::ZoomIn);
    ui->actionZoomOut->setShortcut(QKeySequence::ZoomOut);

    //Document Menu
    connect(ui->menuDocument, SIGNAL(aboutToShow()), this, SLOT(menuDocumentAboutToShow()));
    connect(ui->actionAddChart, SIGNAL(triggered()), this, SLOT(documentNewChart()));

    connect(ui->actionRemoveTab, SIGNAL(triggered()), this, SLOT(removeCurrentTab()));

    //Chart Menu
    connect(ui->menuChart, SIGNAL(aboutToShow()), this, SLOT(menuChartAboutToShow()));
    connect(ui->actionEditName, SIGNAL(triggered()), this, SLOT(chartEditName()));
    
    //Tools Menu
    connect(ui->actionOptions, SIGNAL(triggered()), this, SLOT(toolsOptions()));
    connect(ui->actionRegisterSoftware, SIGNAL(triggered()), this, SLOT(toolsRegisterSoftware()));
    connect(ui->actionStitchLibrary, SIGNAL(triggered()), this, SLOT(toolsStitchLibrary()));
    connect(ui->actionCheckForUpdates, SIGNAL(triggered()), this, SLOT(toolsCheckForUpdates()));
    
    if(!Settings::inst()->isDemoVersion())
        ui->actionRegisterSoftware->setVisible(false);

    //Help Menu
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(helpAbout()));

}

void MainWindow::filePrint()
{
    //TODO: page count isn't working...
    QPrinter *printer = new QPrinter();
    QPrintDialog *dialog = new QPrintDialog(printer, this);

    if(dialog->exec() != QDialog::Accepted)
        return;

    print(printer);
}

void MainWindow::print(QPrinter *printer)
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    
    int tabCount = ui->tabWidget->count();
    QPainter *p = new QPainter();
    
    p->begin(printer);
    
    bool firstPass = true;
    for(int i = 0; i < tabCount; ++i) {
        if(!firstPass)
            printer->newPage();
        
        ChartTab *tab = qobject_cast<ChartTab*>(ui->tabWidget->widget(i));
        tab->renderChart(p);
        firstPass = false;
    }
    p->end();
    
    QApplication::restoreOverrideCursor();
}

void MainWindow::filePrintPreview()
{
    //FIXME: this isn't working
    QPrinter *printer = new QPrinter(QPrinter::HighResolution);
    QPrintPreviewDialog *dialog = new QPrintPreviewDialog(printer, this);
    connect(dialog, SIGNAL(paintRequested(QPrinter *)), this, SLOT(print(QPrinter*)));
    
    dialog->exec();
}

void MainWindow::fileExport()
{
    //TODO: in the future check for all tab types or a base Tab type.
    if(!hasTab())
        return;
    
    ExportUi d(ui->tabWidget, this);
    if(d.exec() != QDialog::Accepted)
        return;

    if(Settings::inst()->isDemoVersion()) {
        Settings::inst()->trialVersionMessage(this);
        return;
    }
    
    QString filter;
    if(d.exportType == "pdf")
        filter = tr("Portable Document Format (pdf)(*.pdf)");
    else if(d.exportType == "svg")
        filter = tr("Scaled Vector Graphics (svg)(*.svg *.svgz)");
    else if(d.exportType == "jpeg")
        filter = tr("Joint Photographic Experts Group (jpeg)(*.jpeg *.jpg)");
    else if(d.exportType == "png")
        filter = tr("Portable Network Graphics (png)(*.png)");
    else if(d.exportType == "gif")
        filter = tr("Graphics Interchange Format (gif)(*.gif)");
    else if(d.exportType == "tiff")
        filter = tr("Tagged Image File Format (tiff)(*.tiff *.tif)");
    else if(d.exportType == "bmp")
        filter = tr("Bitmap (bmp)(*.bmp)");
    else
        filter = tr("");

    QString fileLoc = Settings::inst()->value("fileLocation").toString();
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export Pattern As..."), fileLoc, filter);

    if(fileName.isEmpty())
        return;

    if(d.exportType.isEmpty())
        return;
    
    if(d.exportType == "pdf")
        exportPdf(d.selection, fileName, QSize(d.width, d.height), d.resolution);
    else if(d.exportType == "svg")
        exportSvg(d.selection, fileName, QSize(d.width, d.height));
    else
        exportImg(d.selection, fileName, QSize(d.width, d.height), d.resolution);
}

void MainWindow::exportPdf(QString selection, QString fileName, QSize size, int resolution)
{
    int tabCount = ui->tabWidget->count();
    QPainter *p = new QPainter();
    
    QPrinter *printer = new QPrinter(QPrinter::HighResolution);
    printer->setOutputFormat(QPrinter::PdfFormat);
    printer->setOutputFileName(fileName);
    printer->setResolution(resolution);
    
    p->begin(printer);
    
    bool firstPass = true;
    for(int i = 0; i < tabCount; ++i) {
        if(!firstPass)
            printer->newPage();

        if(selection == tr("All Charts") || selection == ui->tabWidget->tabText(i)) {
            ChartTab *tab = qobject_cast<ChartTab*>(ui->tabWidget->widget(i));
            tab->renderChart(p, QRectF(QPointF(0,0),QSizeF((qreal)size.width(), (qreal)size.height())));
            firstPass = false;
        }
    }
    p->end();
}

void MainWindow::exportSvg(QString selection, QString fileName, QSize size)
{
    int tabCount = ui->tabWidget->count();
    QPainter *p = new QPainter();
    
    QSvgGenerator gen;
    //TODO: fill in more info about the svg.
    gen.setFileName(fileName);
    //FIXME: gen.setSize(mScene->sceneRect().size().toSize());
    
    p->begin(&gen);

    for(int i = 0; i < tabCount; ++i) {
        if(selection == ui->tabWidget->tabText(i)) {
            ChartTab *tab = qobject_cast<ChartTab*>(ui->tabWidget->widget(i));
            tab->renderChart(p, QRectF(QPointF(0,0),QSizeF((qreal)size.width(), (qreal)size.height())));
        }
    }
    p->end();
    
}

void MainWindow::exportImg(QString selection, QString fileName, QSize size, int resolution)
{
    int tabCount = ui->tabWidget->count();
    QPainter *p = new QPainter();
    
    double dpm = resolution * (39.3700787);
    QImage img = QImage(size, QImage::Format_ARGB32); /*mScene->sceneRect().size().toSize()*/
    img.setDotsPerMeterX(dpm);
    img.setDotsPerMeterY(dpm);

    p->begin(&img);
    p->fillRect(0, 0, size.width(), size.height(), QColor(Qt::white));
    
    for(int i = 0; i < tabCount; ++i) {
        if(selection == ui->tabWidget->tabText(i)) {
            ChartTab *tab = qobject_cast<ChartTab*>(ui->tabWidget->widget(i));
            tab->renderChart(p, QRectF(QPointF(0,0),QSizeF((qreal)size.width(), (qreal)size.height())));
        }
    }
    p->end();

    img.save(fileName /*, file type, img quality.*/); 
    
}

//TODO: find out if there is a better place for the legend/rendering fucntions.
void MainWindow::generateStitchLegend(QPainter* painter)
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QStringList keys = mPatternStitches.keys();

    bool drawDescription = Settings::inst()->value("showStitchDescription").toBool();
    bool drawWrongSide = Settings::inst()->value("showWrongSideDescription").toBool();
    int columnCount = Settings::inst()->value("stitchColumnCount").toInt();
    
    foreach(QString key, keys) {
        Stitch *s = StitchCollection::inst()->findStitch(key);
        if(!s) {
            qWarning() << "Couldn't find stitch while generating legend: " << key;
            continue;
        }

        if(s->isSvg())
            s->renderSvg()->render(painter);
        else
            painter->drawPixmap(0,0, *(s->renderPixmap()));

        painter->drawText(0,0, s->name());

        if(drawDescription)
            painter->drawText(0,0, s->description());
        if(drawWrongSide)
            painter->drawText(0,0, s->wrongSide());
        
    }

    QApplication::restoreOverrideCursor();
}

void MainWindow::generateColorLegend(QPainter* painter)
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QStringList keys = mPatternColors.keys();

    bool showHexValues = Settings::inst()->value("showHexValues").toBool();
    int columnCount = Settings::inst()->value("colorColumnCount").toInt();
    QString colorNumber = Settings::inst()->value("colorPrefix").toString();
    
    foreach(QString key, keys) {
        qDebug() << key << mPatternColors.value(key);

        painter->drawPixmap(0,0, drawColorBox(QColor(key), QSize(32, 32)));

        colorNumber += QString::number(mPatternColors.value(key).value("color number"));
        painter->drawText(0,0, colorNumber);
    }
    
    QApplication::restoreOverrideCursor();
    
}

QPixmap MainWindow::drawColorBox(QColor color, QSize size)
{
    QPixmap pix = QPixmap(size);
    QPainter p;
    p.begin(&pix);
    p.fillRect(QRect(QPoint(0, 0), size), QColor(color));
    p.drawRect(1, 1, size.width() -2, size.height() -2);
    p.end();

    return pix;
}


void MainWindow::documentNewChart()
{
    int rows = Settings::inst()->value("defaultRows").toInt();
    int stitches = Settings::inst()->value("defaultStitches").toInt();
    
    ui->rows->setValue(rows);
    ui->stitches->setValue(stitches);
    
    ui->chartTitle->setText(nextChartName());
    ui->newDocument->show();
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
                                .arg(AppInfo::appBuildInfo)
                                .arg(qApp->organizationName())
                                );
    QString fName = Settings::inst()->value("firstName").toString();
    QString lName = Settings::inst()->value("lastName").toString();
    QString email = Settings::inst()->value("email").toString();
    QString sn = Settings::inst()->value("serialNumber").toString();

    QString licenseInfo;

    if(Settings::inst()->isDemoVersion()) {
        licenseInfo = QString(tr("<p>This is a demo version licensed to:<br />"
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

    aboutInfo.append(licenseInfo);
    QMessageBox::about(this, tr("About Crochet"), aboutInfo);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Settings::inst()->setValue("geometry", saveGeometry());
    Settings::inst()->setValue("windowState", saveState());
    QMainWindow::closeEvent(event);
}

void MainWindow::readSettings()
{
    //TODO: For full session restoration reimplement QApplication::commitData()
    //See: http://doc.qt.nokia.com/stable/session.html
    restoreGeometry(Settings::inst()->value("geometry").toByteArray());
    restoreState(Settings::inst()->value("windowState").toByteArray());

}

void MainWindow::toolsOptions()
{
    SettingsUi dialog(this);
    dialog.exec();
}

void MainWindow::fileOpen()
{
    QString fileLoc = Settings::inst()->value("fileLocation").toString();
    QString fileName = QFileDialog::getOpenFileName(this,
         tr("Open Crochet Pattern"), fileLoc, tr("Crochet Pattern (*.pattern)"));

    if(fileName.isEmpty() || fileName.isNull())
        return;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    if(ui->tabWidget->count() > 0) {
        MainWindow *newWin = new MainWindow(0, fileName);
        newWin->move(x() + 40, y() + 40);
        newWin->ui->newDocument->hide();
        newWin->show();
    } else {
        ui->newDocument->hide();
        mFile->fileName = fileName;
        mFile->load();
    }
    QApplication::restoreOverrideCursor();
}

void MainWindow::fileSave()
{
    if(Settings::inst()->isDemoVersion()) {
        Settings::inst()->trialVersionMessage(this);
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
    mFile->fileName = fileName;
    mFile->save();
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

    ui->actionClose->setEnabled(state);
    ui->actionSave->setEnabled(state);
    ui->actionSaveAs->setEnabled(state);

    ui->actionPrint->setEnabled(state);
    ui->actionPrintPreview->setEnabled(state);
    
    ui->actionExport->setEnabled(state);
    
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
    
    ui->actionViewFullScreen->setChecked(isFullScreen());

    bool state = hasTab();
    ui->actionZoomIn->setEnabled(state);
    ui->actionZoomOut->setEnabled(state);
}

void MainWindow::fileNew()
{

    if(ui->tabWidget->count() > 0) {
        MainWindow *newWin = new MainWindow;
        newWin->move(x() + 40, y() + 40);
        newWin->show();
        newWin->ui->newDocument->show();
    } else {
        ui->newDocument->show();
    }
    
}

void MainWindow::newChart()
{
    ui->newDocument->hide();

    int rows = ui->rows->text().toInt();
    int cols = ui->stitches->text().toInt();
    QString defStitch = ui->defaultStitch->currentText();
    QString name = ui->chartTitle->text();
    
    if(docHasChartName(name))
        name = nextChartName(name);

    ChartTab *tab = createTab();
    
    if(name.isEmpty())
        name = nextChartName();
    
    ui->tabWidget->addTab(tab, name);
    ui->tabWidget->setCurrentWidget(tab);
    //FIXME: pass data to tab to pass into scene -- remove the scene header file from this file.
    tab->scene()->createChart(rows, cols); 
}

ChartTab* MainWindow::createTab()
{

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    
    ChartTab* tab = new ChartTab(ui->tabWidget);
    tab->setPatternStitches(&mPatternStitches);
    tab->setPatternColors(&mPatternColors);
    connect(tab, SIGNAL(chartStitchChanged()), this, SLOT(updatePatternStitches()));
    connect(tab, SIGNAL(chartColorChanged()), this, SLOT(updatePatternColors()));

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

void MainWindow::menuDocumentAboutToShow()
{
    bool state = hasTab();

    ui->actionRemoveTab->setEnabled(state);
}

void MainWindow::menuChartAboutToShow()
{
    bool state = hasTab();
    ui->actionEditName->setEnabled(state);       
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
    if(ok && !newName.isEmpty())
        ui->tabWidget->setTabText(curTab, newName);
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
    mUpdater->checkForUpdates(silent);
}

void MainWindow::toolsStitchLibrary()
{
    StitchLibraryUi *d = new StitchLibraryUi(this);
    int value = d->exec();
    
    if(value != QDialog::Accepted) {
        delete d;
        d = 0;
        return;
    }

    qWarning() << "Make this dialog do something.";

    delete d;
    d = 0;

}

void MainWindow::viewZoomIn()
{
    ChartTab* tab = curChartTab();
    if(!tab)
        return;
    tab->zoomIn();
}

void MainWindow::viewZoomOut()
{
    ChartTab* tab = curChartTab();
    if(!tab)
        return;
    tab->zoomOut();
}

ChartTab* MainWindow::curChartTab()
{
    ChartTab* tab = qobject_cast<ChartTab*>(ui->tabWidget->currentWidget());
    return tab;
}

bool MainWindow::hasTab()
{
    ChartTab *cTab = qobject_cast<ChartTab*>(ui->tabWidget->currentWidget());
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

    ChartTab *tab = qobject_cast<ChartTab*>(ui->tabWidget->widget(newTab));
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

    if(ui->tabWidget->count() == 1)
        return;

    //FIXME: either include a warning that this is NOT undo-able or make it undo-able.
    delete ui->tabWidget->widget(tabIndex);
    ui->tabWidget->removeTab(tabIndex);
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
            Stitch* s = StitchCollection::inst()->findStitch(i.key());
            QPixmap pix = QPixmap(QSize(32, 32));
            pix.load(s->file());
            QIcon icon = QIcon(pix);
            QListWidgetItem *item = new QListWidgetItem(icon, i.key(), ui->patternStitches);
            ui->patternStitches->addItem(item);
        }
    }
}

void MainWindow::updatePatternColors()
{
    if(ui->tabWidget->count() <= 0)
        return;
    
    ui->patternColors->clear();
    QMapIterator<QString, QMap<QString, int> > i(mPatternColors);
    while (i.hasNext()) {
        i.next();
        QList<QListWidgetItem*> items = ui->patternColors->findItems(i.key(), Qt::MatchExactly);
        if(items.count() == 0) {
            QPixmap pix = drawColorBox(i.key(), QSize(32, 32));
            QIcon icon = QIcon(pix);           
            QString prefix = Settings::inst()->value("colorPrefix").toString();
            QListWidgetItem *item = new QListWidgetItem(icon, prefix + QString::number(i.value()["color number"]), ui->patternColors);
            item->setToolTip(i.key());
            ui->patternColors->addItem(item);
        }
    }
}
