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

#include <QDir>

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
    setupValueList();
    initDemoVersion();
}

void Settings::initDemoVersion()
{
    QString license = value("license").toString();
    QString sn      = value("serialNumber").toString();
    QString email   = value("email").toString();

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
    QVariant value = mValueList.value(key);
    if(!value.isValid())
        qWarning() << "There is no default value for the option:" << key;
    return value;
}

void Settings::setupValueList() {

    //look up values for setting server/webpage for license and update testing.
    mValueList["licensePage"] = QVariant(AppInfo::liveLicensePage);
    mValueList["updatePage"] = QVariant(AppInfo::liveUpdatePage);
    
    //license information
    mValueList["firstName"] = QVariant("");
    mValueList["lastName"] = QVariant("");
    mValueList["email"] = QVariant("");
    mValueList["serialNumber"] = QVariant("");
    mValueList["license"] = QVariant("");

    QString userDocs = QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation);
    
    //general application options
    mValueList["checkForUpdates"] = QVariant(true);
    mValueList["fileLocation"] = QVariant(userDocs);

    mValueList["geometry"] = QVariant("");
    mValueList["windowState"] = QVariant("");

    //round charts options
    mValueList["defaultStitch"] = QVariant("ch");
    mValueList["defaultPlaceholder"] = QVariant("x");
    mValueList["defaultRows"] = QVariant(15);
    mValueList["defaultStitches"] = QVariant(15);
    
    mValueList["alternateRowColors"] = QVariant(true);
    mValueList["stitchPrimaryColor"] = QVariant("#000000");
    mValueList["stitchAlternateColor"] = QVariant("#D4E6FC");
    mValueList["generateRepeats"] = QVariant(true);
    
    //stitch legend options
    mValueList["stitchColumnCount"] = QVariant(2);
    mValueList["showStitchDescription"] = QVariant(true);
    mValueList["showWrongSideDescription"] = QVariant(false);

    //color legend options
    mValueList["colorPrefix"] = QVariant("C");
    mValueList["colorColumnCount"] = QVariant(2);
    mValueList["showHexValues"] = QVariant(false);

}

QString Settings::userSettingsFolder()
{
    //If we're on windows use the DataLocation as you cannot store files in the registry.
    //Otherwise use the config path to keep all the files together.
#ifdef Q_WS_WIN
    QString folder = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    if(!QFileInfo(folder).exists())
        QDir(folder).mkpath(folder);
    return folder + "/";
#else
    QFileInfo file(mSettings.fileName());
    return file.absolutePath() + "/";
#endif
}

