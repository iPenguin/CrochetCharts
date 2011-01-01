#ifndef APPINFO_H
#define APPINFO_H

#include <QString>
#include <QRegExp>
#include "version.h"

namespace AppInfo {
    const QString appName = "Crochet";
    const QString appOrg = "Stitch Works Software";

    const QString appOrgDomain = "StitchWorksSoftware.com";
    const QString appOrgContact = "http://StitchWorksSoftware.com/contact";
    const QString appVersion = GIT_VERSION;

    const QString appBuildInfo = QString(__DATE__) + " " + QString(__TIME__);

    const QRegExp emailRegExp = QRegExp("[a-zA-Z0-9._%+-]+@(?:[a-zA-Z0-9-]+\\.)+[a-zA-Z]{2,4}");

    //To use licensePage: QString test = QString(liveLicensePage).arg(1).arg(2).arg(3).arg(4);
    //WARNING: NEVER change the value of licensePage to a testing server! if you need to use
    //a testing server for the license set the licensePage variable in the default config file!
    const QString liveLicensePage = "http://stitchworkssoftware.com/license/license.php"
                                "?sws_sn=%1&sws_email=%2&sws_fname=%3&sws_lname=%4";
}

#endif // APPINFO_H
