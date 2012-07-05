/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "settings.h"

#include "license.h"

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
    mRecentFiles = value("recentFiles").toStringList();
}

Settings::~Settings()
{
    setValue("recentFiles", QVariant(mRecentFiles));
}

void Settings::initDemoVersion()
{
#ifndef APPLE_APP_STORE
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
#endif
    mIsDemoVersion = false;

}

void Settings::trialVersionMessage(QWidget* parent)
{
    QMessageBox msgbox(parent);
    msgbox.setWindowTitle(AppInfo::inst()->appName);
    msgbox.setText(tr("This feature is disabled in the demo version."));
    msgbox.setInformativeText(tr("There are example output files and screenshots available at http://%1/%2.")
        .arg(AppInfo::inst()->appOrgDomain).arg(AppInfo::inst()->appName));
    msgbox.setStandardButtons(QMessageBox::Ok);
    msgbox.setIcon(QMessageBox::Information);
    
    msgbox.exec();
}

void Settings::setValue(const QString &key, const QVariant &value)
{
    //only save values that aren't defaults, this allows for undefined values to change with updates,
    //while defined values are fixed.
    if(mValueList[key] != value)
        mSettings.setValue(key, value);
    else
        mSettings.remove(key);

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
    mValueList["licensePage"] = QVariant(AppInfo::inst()->liveLicensePage + AppInfo::inst()->liveLicensePageVals);
    mValueList["updatePage"] = QVariant(AppInfo::inst()->liveUpdatePage + AppInfo::inst()->liveUpdatePageVals);
    mValueList["lastUsed"] = QVariant("");
    
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

    mValueList["maxRecentFiles"] = QVariant(5);
    mValueList["recentFiles"] = QVariant(QStringList());
    
    mValueList["geometry"] = QVariant("");
    mValueList["windowState"] = QVariant(" "); //use a space because it works for the comparison when saving variables.
    
    mValueList["generateTextRepeats"] = QVariant(true);
    mValueList["showChartCenter"] = QVariant(false);

    mValueList["pasteOffset"] = QVariant(tr("Down and Right"));
    
    //charts options
    mValueList["defaultStitch"] = QVariant("ch");
    mValueList["rowCount"] = QVariant(15);
    mValueList["stitchCount"] = QVariant(15);
    mValueList["chartStyle"] = QVariant(tr("Blank"));
    mValueList["increaseBy"] = QVariant(12);
    
    mValueList["useAltColors"] = QVariant(true);
    mValueList["stitchPrimaryColor"] = QVariant("#000000");
    mValueList["stitchAlternateColor"] = QVariant("#3366aa");

    mValueList["chartRowIndicator"] = QVariant(tr("Dots and Text"));
    mValueList["chartIndicatorColor"] = QVariant("#c00000");
    mValueList["showIndicatorOutline"] = QVariant(false);
    
    //stitch legend options
    mValueList["stitchLegendColumnCount"] = QVariant(2);
    mValueList["showStitchDescription"] = QVariant(true);
    mValueList["showStitchWrongSide"] = QVariant(false);
    mValueList["showStitchTitle"] = QVariant(true);
    mValueList["showStitchBorder"] = QVariant(true);

    //color legend options
    mValueList["colorPrefix"] = QVariant("C");
    mValueList["colorLegendColumnCount"] = QVariant(2);
    mValueList["colorLegendSortBy"] = QVariant(tr("Age"));
    mValueList["showColorHexValues"] = QVariant(false);
    mValueList["showColorBorder"] = QVariant(true);
    mValueList["showColorTitle"] = QVariant(true);

}

void Settings::addRecentFile(QString fileName)
{

    if(mRecentFiles.contains(fileName))
        mRecentFiles.removeAll(fileName);
    mRecentFiles.prepend(fileName);
    setValue("recentFiles", QVariant(mRecentFiles));
}

void Settings::setRecentFiles(QStringList files)
{
    mRecentFiles = files;
    setValue("recentFiles", QVariant(mRecentFiles));
}

QString Settings::userSettingsFolder()
{
    QString folder = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    if(!QFileInfo(folder).exists())
        QDir(folder).mkpath(folder);
    return folder + "/";
}

