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
    const QString appBuildInfo;
    
    const QRegExp emailRegExp;
    
    //To use licensePage: QString test = QString(liveLicensePage).arg(1).arg(2).arg(3).arg(4);
    //WARNING: NEVER change the value of the live pages to a testing server! if you need to use
    //a testing server set the licensePage and/or updatePage variables in the default config file!
    const QString liveLicensePage;
    
    const QString liveUpdatePage;
    
    const QString demoString;
    
    const quint32 magicNumber;
    const quint32 magicNumberSet;

private:
    static AppInfo *mInstance;
    
    AppInfo() :
        appName("Crochet"),
        appOrg("Stitch Works Software"),
    
        appOrgDomain("StitchWorksSoftware.com"), 
        appOrgContact("http://StitchWorksSoftware.com/contact"),

        appVersion(gGIT_VERSION),
        appBuildInfo(QString(__DATE__) + " " + QString(__TIME__)),

        emailRegExp(QRegExp("[a-zA-Z0-9._%+-]+@(?:[a-zA-Z0-9-]+\\.)+[a-zA-Z]{2,4}")),

        liveLicensePage("https://stitchworkssoftware.com/custom/license.php"
        "?sws_sn=%1&sws_email=%2&sws_fname=%3&sws_lname=%4"),

        liveUpdatePage("https://stitchworkssoftware.com/custom/update.php"
        "?sws_software=%1&sws_version=%2&sws_os=%3&sws_sn=%4"),

        demoString(QObject::tr("Stitch Works Software - Demo Version  -  ")),

        magicNumber(0x95973530),
        magicNumberSet(0x53095973)
    {
    }
    
};

#endif // APPINFO_H
