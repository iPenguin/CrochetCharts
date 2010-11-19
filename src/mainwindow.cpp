#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_export.h"

#include <QDialog>

#include "charttab.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupMenus();


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupMenus()
{
    //File


    connect(ui->actionExport, SIGNAL(triggered()), this, SLOT(fileExport()));
    connect(ui->actionQuit, SIGNAL(triggered()), qApp, SLOT(quit()));

    //Edit

    //View

    //Document

    connect(ui->actionAdd_Chart, SIGNAL(triggered()), this, SLOT(documentNewChart()));

    //Chart

    //Options

    //Help

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
