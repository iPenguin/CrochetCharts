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
    
    if(key == "defaultStitch") //default stitch to use when filling a new document
        return QVariant("ch");
    
    else if(key == "licensePage") //which server & webpage to look at when getting license information.
        return QVariant(AppInfo::liveLicensePage);
    else if(key == "updatePage") //which server & webpage to look at when checking for updates.
        return QVariant(AppInfo::liveUpdatePage);
    
    else if(key == "firstName") //license info
        return QVariant("");
    else if(key == "lastName") //license info
        return QVariant("");
    else if(key == "email") //license info
        return QVariant("");
    else if(key == "serialNumber") //license info
        return QVariant("");
    else if(key == "license") //license info
        return QVariant("");
    
    else if(key == "checkForUpdates") //check for updates at startup
        return QVariant(true);
    
    else if(key == "defaultRows") //default row count for new docs
        return QVariant(15);
    else if(key == "defaultStitches") //default stitch count for new docs
        return QVariant(15);
    
    else if(key == "fileLocation") { //default location to look for files.
        QString userDocs = QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation);
        return QVariant(userDocs);
    }
    
    else if (key == "geometry") //positioning of window when application was last quit.
        return QVariant();
    else if (key == "windowState") //internal wiget state when application was last quit.
        return QVariant();
    else 
        return QVariant();
}
