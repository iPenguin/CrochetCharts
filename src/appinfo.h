/*************************************************\
| (c) 2011 Stitch Works Software                  |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef APPINFO_H
#define APPINFO_H

#include <QString>
#include <QRegExp>
#include "version.h"
#include <qobject.h>

namespace AppInfo {
    const QString appName = "Crochet";
    const QString appOrg = "Stitch Works Software";

    const QString appOrgDomain = "StitchWorksSoftware.com";
    const QString appOrgContact = "http://StitchWorksSoftware.com/contact";

    const QString appVersion = gGIT_VERSION;
    const QString appBuildInfo = QString(__DATE__) + " " + QString(__TIME__);

    const QRegExp emailRegExp = QRegExp("[a-zA-Z0-9._%+-]+@(?:[a-zA-Z0-9-]+\\.)+[a-zA-Z]{2,4}");

    //To use licensePage: QString test = QString(liveLicensePage).arg(1).arg(2).arg(3).arg(4);
    //WARNING: NEVER change the value of the live pages to a testing server! if you need to use
    //a testing server set the licensePage and/or updatePage variables in the default config file!
    const QString liveLicensePage = "http://stitchworkssoftware.com/license/license.php"
                                "?sws_sn=%1&sws_email=%2&sws_fname=%3&sws_lname=%4";
    
    const QString liveUpdatePage = "http://stitchworkssoftware.com/updates/update.php"
                                "?sws_version=%1&sws_sn=%2";
    
    const QString demoString = QObject::tr("Stitch Works Software - Demo Version  -  ");
    
    const quint32 magicNumber = 0x95973530;
}

#endif // APPINFO_H
