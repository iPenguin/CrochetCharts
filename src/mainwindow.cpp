#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_export.h"

#include <QDialog>
#include <QMessageBox>
#include <QFileDialog>

#include "appinfo.h"
#include "charttab.h"

#include "settings.h"
#include "settingsui.h"

#include "licensewizard.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupMenus();

    this->readSettings();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupMenus()
{
    //File
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(fileOpen()));
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(fileSave()));
    connect(ui->actionSaveAs, SIGNAL(triggered()), this, SLOT(fileSaveAs()));

    connect(ui->actionExport, SIGNAL(triggered()), this, SLOT(fileExport()));
    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));

    //Edit

    //View
    connect(ui->menuView, SIGNAL(aboutToShow()), this, SLOT(menuViewAboutToShow()));
    connect(ui->actionShowStitches, SIGNAL(triggered()), this, SLOT(viewShowStitches()));
    //Document

    connect(ui->actionAddChart, SIGNAL(triggered()), this, SLOT(documentNewChart()));

    //Chart

    //Tools
    connect(ui->actionOptions, SIGNAL(triggered()), this, SLOT(toolsOptions()));

    if(!Settings::inst()->isDemoVersion())
        ui->actionRegisterSoftware->setVisible(false);
    connect(ui->actionRegisterSoftware, SIGNAL(triggered()), this, SLOT(toolsRegisterSoftware()));

    //Help
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(helpAbout()));

}

void MainWindow::fileExport()
{
    QDialog *d = new QDialog(this);
    expDialog = new Ui::ExportDialog();
    expDialog->setupUi(d);

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

void MainWindow::documentNewChart()
{

    ChartTab *newTab = new ChartTab(this);
    ui->tabWidget->addTab(newTab, tr("Chart"));

}

void MainWindow::helpAbout()
{
    QString aboutInfo = QString(tr("<h1>%1</h1>"
                                   "<p>Version: %2 (built on %3)</p>"
                                   "<p>By: %4</p>"
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

    qDebug() << "open the file " << fileName;
}

void MainWindow::fileSave()
{
    QString fileLoc = Settings::inst()->value("fileLocation", QVariant("")).toString();
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save Crochet Pattern"), fileLoc, tr("Crochet Pattern (*.crochet)"));

    if(fileName.isEmpty())
        return;

    qDebug() << "save the file" << fileName;
}

void MainWindow::fileSaveAs()
{

    qDebug() << "save the file as";
}

void MainWindow::menuViewAboutToShow()
{
    ui->actionShowStitches->setChecked(ui->stitchLibraryDock->isVisible());

}

void MainWindow::viewShowStitches()
{
    ui->stitchLibraryDock->setVisible(ui->actionShowStitches->isChecked());
}

void MainWindow::toolsRegisterSoftware()
{
    if(Settings::inst()->isDemoVersion()) {
        LicenseWizard wizard(true, this);
        if(wizard.exec() != QWizard::Accepted) {
                Settings::inst()->setDemoVersion(false);
                return;
        }
    }
}
