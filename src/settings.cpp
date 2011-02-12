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

#include <QMessageBox>

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

void Settings::trialVersionMessage(QWidget *parent)
{
    QMessageBox msgbox(parent);
    msgbox.setWindowTitle(qApp->applicationName());
    msgbox.setText(tr("This feature is disabled in the demo version."));
    msgbox.setInformativeText(tr("There are some example output files and screen shots on the website."));
    msgbox.setStandardButtons(QMessageBox::Ok);
    msgbox.setIcon(QMessageBox::Information);
    
    msgbox.exec();
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

    //general application options
    else if(key == "checkForUpdates")
        return QVariant(true);
    else if(key == "fileLocation") {
        QString userDocs = QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation);
        return QVariant(userDocs);
    }
    else if (key == "geometry")
        return QVariant();
    else if (key == "windowState")
        return QVariant();

    //round charts options
    else if(key == "defaultStitch")
        return QVariant("ch");
    else if(key == "defaultRows")
        return QVariant(15);
    else if(key == "defaultStitches")
        return QVariant(15);
    else if(key == "alternateRowColors")
        return QVariant(true);
    else if(key == "stitchPrimaryColor")
        return QVariant("#000000");
    else if(key == "stitchAlternateColor")
        return QVariant("#D4E6FC");
        
    
    //stitch legend options
    else if(key == "stitchColumnCount")
        return QVariant(2);
    else if(key == "showStitchDescription")
        return QVariant(true);
    else if(key == "showWrongSideDescription")
        return QVariant(false);

    //color legend options
    else if(key == "colorPrefix")
        return QVariant("C");
    else if(key == "colorColumnCount")
        return QVariant(2);
    else if(key == "showHexValues")
        return QVariant(false);
    
    else {
        qWarning() << "Unknown setting requested: " << key;
        return QVariant();
    }
}

QString Settings::userSettingsFolder()
{
    //FIXME: will this work on windows w/ the registry?
    QFileInfo file(mSettings.fileName());
    return file.absolutePath() + "/";
}

