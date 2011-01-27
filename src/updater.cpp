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

#include "appinfo.h"

#include <QDebug>
#include "settings.h"

Updater::Updater(QWidget* parent)
    : QWidget(parent)
{
    QString url = Settings::inst()->value("updatePage", QVariant(AppInfo::liveUpdatePage)).toString();
    mUrl = QUrl(url);
}

Updater::~Updater()
{
}

void Updater::checkForUpdates(bool silent)
{
qDebug() << "checkforupdates start";
    
    if(!silent)
        QMessageBox::information(this, tr("Check for Updates"),
                                 tr("You are currently using the most recent version of this software."),
                                QMessageBox::Ok);
    
}


void Updater::startRequest()
{
    reply = qnam.get(QNetworkRequest(mUrl));
    connect(reply, SIGNAL(finished()), this, SLOT(httpFinished()));
    connect(reply, SIGNAL(readyRead()), this, SLOT(httpReadyRead()));
}

void Updater::downloadFile(QUrl url)
{
    mUrl = url;
    
    QString fName = "swsc_temp.txt";
    QString path = QDesktopServices::storageLocation(QDesktopServices::TempLocation);
    file = new QFile(path + "/" + fName);
    
    if (!file->open(QIODevice::WriteOnly)){
        QMessageBox::information(this, tr("HTTP"), tr("Unable to save the file %1: %2.")
        .arg(file->fileName()).arg(file->errorString()));
        delete file;
        file = 0;
        return;
    }
    
    // schedule the request
    httpRequestAborted = false;
    startRequest();
}

void Updater::httpFinished()
{
    if (httpRequestAborted) {
        if (file) {
            file->close();
            file->remove();
            delete file;
            file = 0;
        }
        reply->deleteLater();
        return;
    }
    if(file->isOpen()) {
        file->flush();
        file->close();
    }
    if(!file->open(QIODevice::ReadOnly)) {
        qWarning() << "Couldn't open the file for reading: " << file->fileName();
        return;
    }
    QString data = file->readAll();
    
    file->flush();
    file->close();
  //TODO: deal with the data.  
    if (reply->error()) {
        file->remove();
        qWarning() << "add a nice dialog box here to explain what happened.";
    } else {
//        emit licenseCompleted(data, false);
    }
    
    reply->deleteLater();
    reply = 0;
    delete file;
    file = 0;
}

void Updater::httpReadyRead()
{
    // this slot gets called every time the QNetworkReply has new data.
    // We read all of its new data and write it into the file.
    // That way we use less RAM than when reading it at the finished()
    // signal of the QNetworkReply
    if (file)
        file->write(reply->readAll());
}