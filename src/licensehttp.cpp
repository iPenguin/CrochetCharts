#include "licensehttp.h"

#include <QtNetwork/QHttp>
#include <QtNetwork/QNetworkRequest>

#include <QDesktopServices>

#include <QFile>
#include <QFileInfo>
#include <QMessageBox>

#include "appinfo.h"

#include <QDebug>

LicenseHttp::LicenseHttp(QWidget *parent) :
    QWidget(parent)
{
}

void LicenseHttp::startRequest()
{
    reply = qnam.get(QNetworkRequest(mUrl));
    connect(reply, SIGNAL(finished()), this, SLOT(httpFinished()));
    connect(reply, SIGNAL(readyRead()), this, SLOT(httpReadyRead()));
}

void LicenseHttp::downloadFile(QUrl url)
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

void LicenseHttp::httpFinished()
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
        qDebug() << "couldn't open the file for reading!";
        return;
    }
    QString data = file->readAll();

    file->flush();
    file->close();

    if (reply->error()) {
        file->remove();
        QMessageBox::information(this, tr("Stitch Works Registration"),
                                 tr("%1 was unable to connect to the server to register this software. "
                                    "Please make sure you are connected to the internet. If you have a firewall "
                                    "running please make sure it allows this software to connect to the internet. "
                                    "If you are still having problems please contact Stitch Works Software at %2")
                                 .arg(AppInfo::appName).arg(AppInfo::appOrgContact));
    } else {
        emit licenseCompleted(data, false);
    }

    reply->deleteLater();
    reply = 0;
    delete file;
    file = 0;
}

void LicenseHttp::httpReadyRead()
{
    // this slot gets called every time the QNetworkReply has new data.
    // We read all of its new data and write it into the file.
    // That way we use less RAM than when reading it at the finished()
    // signal of the QNetworkReply
    if (file)
        file->write(reply->readAll());
}
