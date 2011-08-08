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
    recentFiles = value("recentFiles").toStringList();
}

Settings::~Settings()
{
    setValue("recentFiles", QVariant(recentFiles));
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
    msgbox.setWindowTitle(AppInfo::inst()->appName);
    msgbox.setText(tr("This feature is disabled in the demo version."));
    msgbox.setInformativeText(tr("There are example output files and screen shots available at http://%1/%2.")
        .arg(AppInfo::inst()->appOrgDomain).arg(AppInfo::inst()->appName));
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
    mValueList["licensePage"] = QVariant(AppInfo::inst()->liveLicensePage + AppInfo::inst()->liveLicensePageVals);
    mValueList["updatePage"] = QVariant(AppInfo::inst()->liveUpdatePage + AppInfo::inst()->liveUpdatePageVals);
    
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
    mValueList["windowState"] = QVariant("");

    //instructions
    mValueList["syntaxColor"] = QVariant("#000080");
    mValueList["generateTextRepeats"] = QVariant(true);
    
    //charts options
    mValueList["defaultStitch"] = QVariant("ch");
    mValueList["placeholder"] = QVariant("x");
    mValueList["rowCount"] = QVariant(15);
    mValueList["stitchCount"] = QVariant(15);
    mValueList["chartStyle"] = QVariant(tr("Flat"));
    
    mValueList["useAltColors"] = QVariant(true);
    mValueList["stitchPrimaryColor"] = QVariant("#000000");
    mValueList["stitchAlternateColor"] = QVariant("#3366AA");

    mValueList["chartRowIndicator"] = QVariant(tr("Dots and Text"));
    mValueList["chartIndicatorColor"] = QVariant("#C00000");
    
    //stitch legend options
    mValueList["stitchLegendColumnCount"] = QVariant(2);
    mValueList["showStitchDescription"] = QVariant(true);
    mValueList["showStitchWrongSide"] = QVariant(false);
    mValueList["showStitchTitle"] = QVariant(true);
    mValueList["showStitchBorder"] = QVariant(true);
    mValueList["showStitchBlock"] = QVariant(true);

    //color legend options
    mValueList["colorPrefix"] = QVariant("C");
    mValueList["colorLegendColumnCount"] = QVariant(2);
    mValueList["colorLegendSortBy"] = QVariant(tr("Age"));
    mValueList["showColorHexValues"] = QVariant(false);
    mValueList["showColorBorder"] = QVariant(true);
    mValueList["showColorTitle"] = QVariant(true);

}

QString Settings::userSettingsFolder()
{
    QString folder = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    if(!QFileInfo(folder).exists())
        QDir(folder).mkpath(folder);
    return folder + "/";
}

