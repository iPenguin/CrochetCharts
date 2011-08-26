/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef UPDATER_H
#define UPDATER_H

#include <QWidget>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QUrl>

#include <QProgressDialog>

class QFile;

class Updater : public QWidget
{
    Q_OBJECT
public:
    Updater(QWidget* parent = 0);
    ~Updater();

    void checkForUpdates(bool silent = false);
private:
    bool mSilent;

public:
    void downloadInstaller(QUrl url);
    
private slots:
    void startRequest();
    void httpFinished();
    void httpReadyRead();

    void httpFinishedInstaller();
    void httpReadyReadInstaller();
    void launchInstaller();

    void cancelDownload();
    void updateDataTransferProgress(qint64 readBytes, qint64 totalBytes);
    
private:
    QUrl mUrl;
    QNetworkAccessManager qnam;
    QNetworkReply* reply,
                 * instReply;

    QProgressDialog* mProgDialog;
    
    QByteArray mData;
    QFile* installer;
    
    int httpGetId;
    bool httpRequestAborted;
};
#endif //UPDATER_H
