/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "settings.h"

#include "license.h"

#include <QApplication>
#include <QDebug>
#include "appinfo.h"
#include <QDesktopServices>
#include <QFileInfo>

// Global static pointer
Settings* Settings::mInstance = NULL;

// singleton constructor:
Settings* Settings::inst()
{
   if (!mInstance)   // Only allow one instance of the settings.
      mInstance = new Settings();
   return mInstance;
}

Settings::Settings()
{
    initDemoVersion();
}

void Settings::initDemoVersion()
{
    QString license = mSettings.value("license", QVariant("")).toString();
    QString sn      = mSettings.value("serialNumber", QVariant("")).toString();
    QString email   = mSettings.value("email", QVariant("")).toString();

    if(!License::isValidSerialNumber(sn)) {
        mIsDemoVersion = true;
        return;
    }
    if(!License::isValidLicense(license, sn, email)) {
        mIsDemoVersion = true;
        return;
    }

    mIsDemoVersion = false;
}

void Settings::setValue(const QString &key, const QVariant &value)
{
    mSettings.setValue(key, value);
}

QVariant Settings::value(const QString &key) const
{
    return mSettings.value(key, defaultValue(key));
}

QVariant Settings::defaultValue ( const QString& key ) const
{
    //look up values for setting server/webpage for license and update testing.
    if(key == "licensePage") 
        return QVariant(AppInfo::liveLicensePage);
    else if(key == "updatePage")
        return QVariant(AppInfo::liveUpdatePage);

    //license information
    else if(key == "firstName")
        return QVariant("");
    else if(key == "lastName")
        return QVariant("");
    else if(key == "email")
        return QVariant("");
    else if(key == "serialNumber")
        return QVariant("");
    else if(key == "license")
        return QVariant("");

    //check for updates at startup
    else if(key == "checkForUpdates")
        return QVariant(true);

    //default options for round charts
    else if(key == "defaultStitch")
        return QVariant("ch");
    else if(key == "defaultRows")
        return QVariant(15);
    else if(key == "defaultStitches")
        return QVariant(15);

    //default location to look for files.
    else if(key == "fileLocation") {
        QString userDocs = QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation);
        return QVariant(userDocs);
    }

    //window state and positioning info.
    else if (key == "geometry")
        return QVariant();
    else if (key == "windowState")
        return QVariant();
    else 
        return QVariant();
}

QString Settings::userSettingsFolder()
{
    //FIXME: will this work on windows w/ the registry?
    QFileInfo file(mSettings.fileName());
    return file.absolutePath() + "/";
}

