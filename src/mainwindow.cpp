/*************************************************\
| (c) 2010-2011 Stitch Works Software             |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_newdocument.h"

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

MainWindow::MainWindow(QWidget *parent, QString fileName)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupMenus();

    setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
    
    setupStitchPalette();
    readSettings();

    mFile = new SaveFile(ui->tabWidget);
    if(!fileName.isEmpty()) {
        mFile->fileName = fileName;
        mFile->load();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
    delete StitchCollection::inst();
}

void MainWindow::setupStitchPalette()
{
    StitchSet *set = StitchCollection::inst()->masterStitchSet();
    ui->stitchPalette->setModel(set);

    //TODO: setup a proxywidget that can hold header sections?
    //StitchPaletteDelegate *delegate = new StitchPaletteDelegate(ui->stitchPalette);
    //ui->stitchPalette->setItemDelegate(delegate);
}

void MainWindow::setupMenus()
{
    //File Menu
    connect(ui->menuFile, SIGNAL(aboutToShow()), this, SLOT(menuFileAboutToShow()));
    connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(fileNew()));
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(fileOpen()));
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(fileSave()));
    connect(ui->actionSaveAs, SIGNAL(triggered()), this, SLOT(fileSaveAs()));
    connect(ui->actionExport, SIGNAL(triggered()), this, SLOT(fileExport()));
    connect(ui->actionClose, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionQuit, SIGNAL(triggered()), qApp, SLOT(quit()));
    
    ui->actionOpen->setIcon(QIcon::fromTheme("document-open" /*, QIcon(":/file-open.png")*/));
    ui->actionNew->setIcon(QIcon::fromTheme("document-new"));
    ui->actionSave->setIcon(QIcon::fromTheme("document-save"));
    ui->actionSaveAs->setIcon(QIcon::fromTheme("document-save-as"));
    ui->actionClose->setIcon(QIcon::fromTheme("document-close"));

    //Edit Menu
    connect(ui->menuEdit, SIGNAL(aboutToShow()), this, SLOT(menuEditAboutToShow()));
    
    ui->actionCopy->setIcon(QIcon::fromTheme("edit-copy" /*, QIcon(":/edit-copy.png")*/));
    ui->actionCut->setIcon(QIcon::fromTheme("edit-cut" /*, QIcon(":/edit-cut.png")*/));
    ui->actionPaste->setIcon(QIcon::fromTheme("edit-paste" /*, QIcon(":/edit-paste.png")*/));

    //View Menu
    connect(ui->menuView, SIGNAL(aboutToShow()), this, SLOT(menuViewAboutToShow()));
    connect(ui->actionShowStitches, SIGNAL(triggered()), this, SLOT(viewShowStitches()));
    connect(ui->actionViewFullScreen, SIGNAL(triggered(bool)), this, SLOT(viewFullScreen(bool)));

    connect(ui->actionZoomIn, SIGNAL(triggered(bool)), this, SLOT(viewZoomIn()));
    connect(ui->actionZoomOut, SIGNAL(triggered(bool)), this, SLOT(viewZoomOut()));
    
    ui->actionZoomIn->setIcon(QIcon::fromTheme("zoom-in"));
    ui->actionZoomOut->setIcon(QIcon::fromTheme("zoom-out"));

    //Document Menu
    connect(ui->actionAddChart, SIGNAL(triggered()), this, SLOT(documentNewChart()));

    //Chart Menu

    //Tools Menu
    connect(ui->actionOptions, SIGNAL(triggered()), this, SLOT(toolsOptions()));
    connect(ui->actionRegisterSoftware, SIGNAL(triggered()), this, SLOT(toolsRegisterSoftware()));
    connect(ui->actionStitchLibrary, SIGNAL(triggered()), this, SLOT(toolsStitchLibrary()));
    
    if(!Settings::inst()->isDemoVersion())
        ui->actionRegisterSoftware->setVisible(false);

    //Help Menu
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(helpAbout()));

}

void MainWindow::fileExport()
{
    //TODO: in the future check for all tab types or a base Tab type.
    if(!hasTab())
        return;
    
    //TODO: pass the tab, or tabbar into the dialog so it can work on the data.
    ExportUi d(this);
    if(d.exec() == QDialog::Accepted) {
        QString fileLoc = Settings::inst()->value("fileLocation", QVariant("")).toString();
        QString fileName = QFileDialog::getSaveFileName(this,
             tr("Export Pattern As..."), fileLoc, tr(""));
    }
}

void MainWindow::documentNewChart()
{
    createChart();
}

void MainWindow::helpAbout()
{
    QString aboutInfo = QString(tr("<h1>%1</h1>"
                                   "<p>Version: %2 (built on %3)</p>"
                                   "<p>(c) 2010-2011 %4</p>"
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
    restoreGeometry(Settings::inst()->value("geometry", QVariant()).toByteArray());
    restoreState(Settings::inst()->value("windowState", QVariant()).toByteArray());

}

void MainWindow::toolsOptions()
{
    SettingsUi dialog;
    dialog.exec();
}

void MainWindow::fileOpen()
{
    QString fileLoc = Settings::inst()->value("fileLocation", QVariant("")).toString();
    QString fileName = QFileDialog::getOpenFileName(this,
         tr("Open Crochet Pattern"), fileLoc, tr("Crochet Pattern (*.pattern)"));

    if(fileName.isEmpty() || fileName.isNull())
        return;

    MainWindow *newWin = new MainWindow(0, fileName);
    newWin->move(x() + 40, y() + 40);
    newWin->show();    
}

void MainWindow::fileSave()
{
    if(Settings::inst()->isDemoVersion()) {
        this->trialVersionMessage();
        return;
    }

    if(mFile->fileName.isEmpty())
        fileSaveAs();
    else {
        SaveFile::FileError err = mFile->save();
        if(err != SaveFile::No_Error)
            qWarning() << "There was an error saving the file: " << err;
    }
    
}

void MainWindow::fileSaveAs()
{
    if(Settings::inst()->isDemoVersion()) {
        this->trialVersionMessage();
        return;
    }

    QString fileLoc = Settings::inst()->value("fileLocation", QVariant("")).toString();
    QString fileName = QFileDialog::getSaveFileName(this,
            tr("Save Crochet Pattern"), fileLoc, tr("Crochet Pattern (*.pattern)"));

    if(fileName.isEmpty())
        return;

    mFile->fileName = fileName;
    mFile->save();
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
    ui->actionShowStitches->setChecked(ui->stitchPaletteDock->isVisible());
    ui->actionViewFullScreen->setChecked(isFullScreen());

    bool state = hasTab();
    ui->actionZoomIn->setEnabled(state);
    ui->actionZoomOut->setEnabled(state);
}

void MainWindow::fileNew()
{
    MainWindow *newWin = new MainWindow;
    newWin->move(x() + 40, y() + 40);
    newWin->show();
    newWin->createChart();
}

void MainWindow::createChart()
{
    QDialog *d = new QDialog(this);
    newDoc = new Ui::NewDocument;
    newDoc->setupUi(d);
    connect(newDoc->buttonBox, SIGNAL(accepted()), d, SLOT(accept()));
    connect(newDoc->buttonBox, SIGNAL(rejected()), d, SLOT(reject()));
    
    if(d->exec() != QDialog::Accepted)
        return;

    int rows = newDoc->rows->text().toInt();
    int cols = newDoc->stitches->text().toInt();
    QString defStitch = newDoc->defaultStitch->currentText();
    QString name = newDoc->chartTitle->text();
    
    ChartTab* tab = new ChartTab(ui->tabWidget);

    tab->scene()->createChart(rows, cols);

    if(name.isEmpty())
        name = tr("Chart");
    
    ui->tabWidget->addTab(tab, name);
}

void MainWindow::viewShowStitches()
{
    ui->stitchPaletteDock->setVisible(ui->actionShowStitches->isChecked());
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

void MainWindow::trialVersionMessage()
{
    QMessageBox msgbox(this);
    msgbox.setWindowTitle(qApp->applicationName());
    msgbox.setText(tr("This feature is disabled in the demo version."));
    msgbox.setInformativeText(tr("There are some example output files and screen shots on the website."));
    msgbox.setStandardButtons(QMessageBox::Ok);
    msgbox.setIcon(QMessageBox::Information);

    msgbox.exec();

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
