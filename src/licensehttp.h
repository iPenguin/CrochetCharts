/*************************************************\
| (c) 2010-2011 Stitch Works Software             |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef LICENSEHTTP_H
#define LICENSEHTTP_H

#include <QWidget>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QUrl>

class QFile;

class LicenseHttp : public QWidget
{
    Q_OBJECT

public:
    LicenseHttp(QWidget *parent = 0);
    void downloadFile(QUrl url);

signals:
    void licenseCompleted(QString license, bool errors);

private slots:
    void startRequest();
    void httpFinished();
    void httpReadyRead();

private:
    QUrl mUrl;
    QNetworkAccessManager qnam;
    QNetworkReply *reply;
    QFile *file;
    int httpGetId;
    bool httpRequestAborted;
};

#endif //LICESNEHTTP_H
