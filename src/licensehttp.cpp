/****************************************************************************\
 Copyright (c) 2010-2014 Stitch Works Software
 Brian C. Milco <bcmilco@gmail.com>

 This file is part of Crochet Charts.

 Crochet Charts is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Crochet Charts is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Crochet Charts. If not, see <http://www.gnu.org/licenses/>.

 \****************************************************************************/
#include "licensehttp.h"

#include <QtNetwork/QHttp>
#include <QtNetwork/QNetworkRequest>

#include <QMessageBox>

#include "appinfo.h"

#include <QDebug>
#include <QApplication>

LicenseHttp::LicenseHttp(QWidget* parent) :
    QWidget(parent)
{
}

void LicenseHttp::downloadFile(QUrl url)
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    mUrl = url;

    // schedule the request
    httpRequestAborted = false;
    startRequest();
}

void LicenseHttp::startRequest()
{
    reply = qnam.get(QNetworkRequest(mUrl));
    connect(&qnam, SIGNAL(finished(QNetworkReply*)), SLOT(httpFinished(QNetworkReply*)));
}

void LicenseHttp::httpFinished(QNetworkReply* reply)
{   
    QString data = reply->readAll();

    if (reply->error()) {
        QApplication::restoreOverrideCursor();
        QMessageBox::information(this, tr("%1 Registration").arg(AppInfo::inst()->appOrg),
                                 tr("%1 was unable to connect to the server to register this software. "
                                    "Please make sure you are connected to the internet. If you have a firewall "
                                    "running please make sure it allows %1 to connect to the internet. "
                                    "If you are still having problems please contact %2 using the form at %3")
                                 .arg(AppInfo::inst()->appName).arg(AppInfo::inst()->appOrg).arg(AppInfo::inst()->appOrgContact));
        return;
    }
   
    reply->deleteLater();
    reply = 0;
    QApplication::restoreOverrideCursor();
    emit licenseCompleted(data, false);
}
