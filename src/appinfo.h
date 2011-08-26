/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef APPINFO_H
#define APPINFO_H

#include "version.h"
#include <QString>
#include <QRegExp>
#include <qobject.h>

/**
 * singleton class to supply general application-wide info to all classes.
 */

class AppInfo
{
public:
    static AppInfo* inst()
    {
        if (!mInstance)   // Only allow one instance of the settings.
      mInstance = new AppInfo();
      return mInstance;
    }
    
    const QString appName;
    const QString appOrg;
    
    const QString appOrgDomain;
    const QString appOrgContact;
    
    const QString appVersion;
    const QString appVersionShort;
    const QString appBuildInfo;
    
    const QRegExp emailRegExp;
    
    /**
     * To use licensePage: QString test = QString(liveLicensePage + liveLicensePageVals).arg(1).arg(2).arg(3).arg(4);\n
     * WARNING: NEVER change the value of the live pages to a testing server! if you need to use\n
     * a testing server set the licensePage and/or updatePage variables in the default config file!\n
     */
    const QString liveLicensePage;
    const QString liveLicensePageVals;

    const QString liveUpdatePage;
    const QString liveUpdatePageVals;

    const QString demoString;
    
    const quint32 magicNumber;
    const quint32 magicNumberSet;

private:
    static AppInfo* mInstance;
    
    AppInfo();
    
};

#endif // APPINFO_H
