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

    //Document

    connect(ui->actionAdd_Chart, SIGNAL(triggered()), this, SLOT(documentNewChart()));

    //Chart

    //Tools
    connect(ui->actionOptions, SIGNAL(triggered()), this, SLOT(optionsUi()));

    //Help
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));

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

void MainWindow::about()
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
    QString name = Settings::inst()->value("name").toString();
    QString email = Settings::inst()->value("email").toString();
    QString sn = Settings::inst()->value("serialNumber").toString();

    QString licenseInfo = QString(tr("<p>This software is licensed to:<br />"
                                     "Name: %1<br />"
                                     "Email: %2<br />"
                                     "Serial #: %3</p>")
                                  .arg(name).arg(email).arg(sn));
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

void MainWindow::optionsUi()
{
    qDebug() << "optionsUi";
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
