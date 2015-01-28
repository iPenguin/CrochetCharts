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
#ifndef LICENSEHTTP_H
#define LICENSEHTTP_H

#include <QWidget>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QUrl>

class LicenseHttp : public QWidget
{
    Q_OBJECT

public:
    LicenseHttp(QWidget* parent = 0);
    void downloadFile(QUrl url);

signals:
    void licenseCompleted(QString license, bool errors);

private slots:
    void startRequest();
    void httpFinished(QNetworkReply* reply);

private:
    QUrl mUrl;
    QNetworkAccessManager qnam;
    QNetworkReply* reply;
    int httpGetId;
    bool httpRequestAborted;
};

#endif //LICESNEHTTP_H
