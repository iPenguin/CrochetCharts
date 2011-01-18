/*************************************************\
| (c) 2010-2011 Stitch Works Software             |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDialog>
#include <QMessageBox>
#include <QFileDialog>

#include <QXmlStreamWriter>

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

#include <QDomDocument>
#include <QDomNode>
#include <QDomElement>

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupMenus();

    setupStitchPalette();

    this->readSettings();
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
    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
    
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
//TODO: include the prompt for document size etc.
    ChartTab *newTab = new ChartTab(this);
    ui->tabWidget->addTab(newTab, tr("Chart"));

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
         tr("Open Crochet Pattern"), fileLoc, tr("Crochet Pattern (*.crochet)"));

    if(fileName.isEmpty())
        return;
    //TODO: open the file
    qWarning() << "TODO: open the file " << fileName;
}

void MainWindow::fileSave()
{
    if(Settings::inst()->isDemoVersion()) {
        this->trialVersionMessage();
        return;
    }

    if(mFileName.isEmpty() || mFileName.isNull())
        fileSaveAs();
    else
        save(mFileName);
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

    save(fileName);
}

bool MainWindow::save(QString fileName)
{
    int tabCount = ui->tabWidget->count();

    QString *data = new QString();
    
    QXmlStreamWriter stream(data);
    stream.setAutoFormatting(true);
    stream.writeStartDocument();

    stream.writeStartElement("pattern"); //start pattern

    //TODO: Write the data to a QDataStream that is versioned and that also
    //contains the custom stitches.
    //foreach(Stitch *s, mUsedCustomStitches)
    //      s->save(stream);
    //      saveIcon(dataStream, s->file()); //copy the icon files into the data stream
    //dataStream << stream(customIcons);
    
    for(int i = 0; i < tabCount; ++i) {
        ChartTab* tab = qobject_cast<ChartTab*>(ui->tabWidget->widget(i));
        if(!tab)
            continue;
        tab->save(&stream);
    }

    stream.writeEndElement(); // end pattern
    stream.writeEndDocument();

    QFile file(fileName);

    if(!file.open(QIODevice::WriteOnly)) {
        //TODO: some nice dialog to warn the user.
        qWarning() << "Couldn't open file for writing..." << fileName;
        return false;
    }

    file.write(data->toLatin1());

    delete data;
    data = 0;
    return true;
}

bool MainWindow::load(QString fileName)
{
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open the file for reading" << fileName;
        //TODO: Add a nice error message.
        return;
    }
    
    QDomDocument doc("pattern");
    
    if (!doc.setContent(&file)) {
        file.close();
        return;
    }
    file.close();
    
    QDomElement docElem = doc.documentElement();
    
    QDomNode n = docElem.firstChild();
    while(!n.isNull()) {
        QDomElement e = n.toElement(); // try to convert the node to an element.
        if(!e.isNull()) {
            
            if(e.tagName() == "name")
                this->setName(e.text());
            else if(e.tagName() == "chart")
                loadXmlChart(e);
            else
                qWarning() << "Could not load part of the stitch set:" << e.tagName() << e.text();
        }
        n = n.nextSibling();
    }
}

void MainWindow::loadXmlChart(QDomElement element)
{
    ChartTab* tab = new ChartTab(ui->tabWidget);
    
    QDomNode n = element.firstChild();
    while(!n.isNull()) {
        QDomElement e = n.toElement();
        if(!e.isNull()) {
            if(e.tagName() == "name") {
                tab->setName(e.text());
            } else if(e.tagName() == "cell") {
                continue;
            } else {
                qWarning() << "Cannot load unknown stitch property:" << e.tagName() << e.text();
            }
        }
        n = n.nextSibling();
    }

    ui->tabWidget->addTab(tab, tab->name());
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
