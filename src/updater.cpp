/*************************************************\
| (c) 2011 Stitch Works Software                  |
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

#include "appinfo.h"

#include <QDebug>
#include "settings.h"

Updater::Updater(QWidget* parent)
    : QWidget(parent), mSilent(false)
{
    QString url = Settings::inst()->value("updatePage", QVariant(AppInfo::liveUpdatePage)).toString();

    QString os;
#if defined(Q_OS_WIN32)
    os = "windows";
#elif defined(Q_OS_LINUX)
    os = "linux";
#elif defined(Q_OS_DARWIN)
    os = "osx";
#endif

    QString sn = Settings::inst()->value("serialNumber", QVariant("")).toString();
    //software, version, os, serial number
    mUrl = QUrl(QString(url).arg(AppInfo::appName.toLower()).arg(AppInfo::appVersion).arg(os).arg(sn));

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
        
        qWarning() << "add a nice dialog box here to explain what happened.";
    } else {

        QStringList urls = data.split("::", QString::SkipEmptyParts);
        if(urls.count() == 2) {
            QMessageBox msgbox(this);
            msgbox.setIcon(QMessageBox::Information);
            msgbox.setText(tr("There is a new version of %1.").arg(AppInfo::appName));
            msgbox.setInformativeText(tr("Would you like to download the new version?"));
            /*QPushButton *downloadNow =*/ msgbox.addButton(tr("Download the new version"), QMessageBox::ActionRole);
            QPushButton *seeNotes    = msgbox.addButton(tr("See what's changed"), QMessageBox::HelpRole);
            QPushButton *remindLater = msgbox.addButton(tr("Remind me later"), QMessageBox::RejectRole);

            msgbox.exec();

            if(msgbox.clickedButton() == remindLater)
                return;
            if(msgbox.clickedButton() == seeNotes) {
                QDesktopServices::openUrl(QUrl(urls.last()));
                return;
            }
                

            qDebug() << "start the download";
            
            
        } else if(!mSilent) {
            QMessageBox::information(this, tr("Check for Updates"),
                            tr("You are using the current version of %1.").arg(AppInfo::appName), QMessageBox::Ok);
        }
    }
    
    reply->deleteLater();
    reply = 0;
}

void Updater::httpReadyRead()
{
    // this slot gets called every time the QNetworkReply has new data.
    // We read all of its new data and write it into the file.
    // That way we use less RAM than when reading it at the finished()
    // signal of the QNetworkReply
    mData.append(reply->readAll());
}

void Updater::downloadInstaller(QUrl url)
{

}
