/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "updater.h"

#include <QtNetwork/QHttp>
#include <QtNetwork/QNetworkRequest>

#include <QDesktopServices>

#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QPushButton>

#include <QProcess>

#include "appinfo.h"
#include <QApplication>

#include <QDebug>
#include "settings.h"

Updater::Updater(QWidget* parent)
    : QWidget(parent), mSilent(false)
{
    QString url = Settings::inst()->value("updatePage").toString();

    QString os;
#if defined(Q_OS_WIN32)
    os = "windows";
#elif defined(Q_OS_LINUX)
    os = "linux";
#elif defined(Q_OS_DARWIN)
    os = "osx";
#endif

    QString sn = Settings::inst()->value("serialNumber").toString();
    //software, version, os, serial number
    mUrl = QUrl(QString(url).arg(AppInfo::appName.toLower()).arg(AppInfo::appVersion).arg(os).arg(sn));

    mProgDialog = new QProgressDialog(this);
}

Updater::~Updater()
{
}

void Updater::checkForUpdates(bool silent)
{
    mSilent = silent;
       
    // schedule the request
    httpRequestAborted = false;
    startRequest();
    
}

void Updater::startRequest()
{
    reply = qnam.get(QNetworkRequest(mUrl));
    connect(reply, SIGNAL(finished()), this, SLOT(httpFinished()));
    connect(reply, SIGNAL(readyRead()), this, SLOT(httpReadyRead()));
}

void Updater::httpFinished()
{
    if (httpRequestAborted) {
        reply->deleteLater();
        return;
    }

    QString data =QString(mData);
        
    if (reply->error()) {
        //TODO: add a warning.
        qWarning() << "add a nice dialog box here to explain what happened.";
    } else {

        QStringList urls = data.split("::", QString::SkipEmptyParts);
        if(urls.count() == 2) {
            QMessageBox msgbox(this);
            msgbox.setIcon(QMessageBox::Information);
            msgbox.setText(tr("There is a new version of %1.").arg(AppInfo::appName));
            msgbox.setInformativeText(tr("Would you like to download the new version?"));
            /*QPushButton *downloadNow =*/ msgbox.addButton(tr("Download the new version"), QMessageBox::ActionRole);
            QPushButton *seeNotes    = msgbox.addButton(tr("See what has changed"), QMessageBox::HelpRole);
            QPushButton *remindLater = msgbox.addButton(tr("Remind me later"), QMessageBox::RejectRole);

            msgbox.exec();

            if(msgbox.clickedButton() == remindLater)
                return;
            if(msgbox.clickedButton() == seeNotes) {
                QDesktopServices::openUrl(QUrl(urls.last()));
                return;
            }
                
            downloadInstaller(QUrl(urls.first()));
            
        } else if(!mSilent) {
            QMessageBox::information(this, tr("Check for Updates"),
                            tr("There are no updates at this time.").arg(AppInfo::appName), QMessageBox::Ok);
        }
    }
    
    reply->deleteLater();
    reply = 0;
}

void Updater::httpReadyRead()
{
    mData.append(reply->readAll());
}

void Updater::downloadInstaller(QUrl url)
{   
    QString fName = url.path().split("/").last();
    QString path = QDesktopServices::storageLocation(QDesktopServices::TempLocation);
    installer = new QFile(path + "/" + fName);
    
    if (!installer->open(QIODevice::WriteOnly)){
        QMessageBox::information(this, tr("HTTP"), tr("Unable to save the file %1: %2.")
        .arg(installer->fileName()).arg(installer->errorString()));
        delete installer;
        installer = 0;
        return;
    }
    
    mProgDialog->setLabelText(tr("Downloading %1").arg(fName));
    connect(mProgDialog, SIGNAL(canceled()), this, SLOT(cancelDownload()));
    mProgDialog->show();
    
    instReply = qnam.get(QNetworkRequest(url));
    connect(instReply, SIGNAL(downloadProgress(qint64,qint64)),
            this, SLOT(updateDataTransferProgress(qint64,qint64)));
    connect(instReply, SIGNAL(finished()), this, SLOT(httpFinishedInstaller()));
    connect(instReply, SIGNAL(readyRead()), this, SLOT(httpReadyReadInstaller()));

}

void Updater::cancelDownload()
{
    httpRequestAborted = true;
    //FIXME:crash on cancel.
    //TODO: add some error handeling so it doesn't just sit there when it's not working.
    instReply->abort();
}

void Updater::updateDataTransferProgress(qint64 readBytes, qint64 totalBytes)
{
    mProgDialog->setMaximum(totalBytes);
    mProgDialog->setValue(readBytes);
}

void Updater::httpFinishedInstaller()
{
    if (httpRequestAborted) {
        if (installer) {
            installer->close();
            installer->remove();
            delete installer;
            installer = 0;
        }
        instReply->deleteLater();
        return;
    }
    
    if(installer->isOpen()) {
        installer->flush();
        installer->close();
    }

    mProgDialog->hide();
    if (instReply->error()) {
        installer->remove();
        //TODO: prompt user that the download failed.
        qWarning() << "failed to download the file.";
    } else {
        launchInstaller();
    }

    instReply->deleteLater();
    instReply = 0;
    delete installer;
    installer = 0;

}

void Updater::httpReadyReadInstaller()
{
    if (installer)
        installer->write(instReply->readAll());
}

void Updater::launchInstaller()
{

    QProcess *installProc = new QProcess(this);

    QString program;
#if defined(Q_OS_WIN32)
    program = installer->fileName();
#elif defined(Q_OS_LINUX)
    program = "sh " + installer->fileName();
#elif defined(Q_OS_DARWIN)
    program = installer->fileName(); //TODO: make sure this launches the dmg file correctly.
#endif

    installProc->startDetached(program);
    installProc->waitForStarted();
    qApp->quit();
}
