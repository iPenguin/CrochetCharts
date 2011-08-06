#include "appinfo.h"

AppInfo* AppInfo::mInstance = NULL;

AppInfo::AppInfo() :
    appName("Crochet Charts"),
    appOrg("Stitch Works Software"),
    
    appOrgDomain("StitchWorksSoftware.com"), 
    appOrgContact("http://StitchWorksSoftware.com/contact"),

    appVersion(gGIT_VERSION),
    appVersionShort(gGIT_VERSION_SHORT),
    appBuildInfo(QString(__DATE__) + " " + QString(__TIME__)),

    emailRegExp(QRegExp("[a-zA-Z0-9._%+-]+@(?:[a-zA-Z0-9-]+\\.)+[a-zA-Z]{2,4}")),

    liveLicensePage("http://stitchworkssoftware.com/custom/license.php"
        "?sws_sn=%1&sws_email=%2&sws_fname=%3&sws_lname=%4"),

    liveUpdatePage("http://stitchworkssoftware.com/custom/update.php"
        "?sws_software=%1&sws_version=%2&sws_os=%3&sws_sn=%4"),

    demoString(QObject::tr("Stitch Works Software - Demo Version  -  ")),

    magicNumber(0x95973530),
    magicNumberSet(0x53095973)
{
}
