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
#include "settings.h"

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
    mRecentFiles = value("recentFiles").toStringList();
}

Settings::~Settings()
{
    setValue("recentFiles", QVariant(mRecentFiles));
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
    mValueList["updatePage"] = QVariant(AppInfo::inst()->liveUpdatePage + AppInfo::inst()->liveUpdatePageVals);
    mValueList["lastUsed"] = QVariant("");
    
    //license information
    mValueList["firstName"] = QVariant("");
    mValueList["lastName"] = QVariant("");
    mValueList["email"] = QVariant("");
    mValueList["serialNumber"] = QVariant("");

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
    mValueList["cellHeight"] = QVariant(64);
    mValueList["cellWidth"] = QVariant(64);
    mValueList["chartStyle"] = QVariant(tr("Blank"));
    mValueList["increaseBy"] = QVariant(12);
    
    mValueList["useAltColors"] = QVariant(true);
    mValueList["stitchPrimaryColor"] = QVariant("#000000");
    mValueList["stitchAlternateColor"] = QVariant("#3366aa");

    mValueList["chartRowIndicator"] = QVariant(tr("Dots and Text"));
    mValueList["chartIndicatorColor"] = QVariant("#c00000");
    mValueList["showIndicatorOutline"] = QVariant(false);
	
	mValueList["replaceStitchWithPress"] = QVariant(true);
    
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

