#ifndef APPINFO_H
#define APPINFO_H

#include "../VERSION"
#include <QString>
#include <QRegExp>

namespace AppInfo {
    const QString appName = "Crochet";
    const QString appOrg = "Stitch Works Software";

    const QString appOrgDomain = "stitchworkssoftware.com";
    const QString appVersion = GIT_VERSION; //ie: 0.1.1

    const QString appBuildInfo = QString(__DATE__) + " " + QString(__TIME__);

    const QRegExp emailRegExp = QRegExp("[a-zA-Z0-9._%+-]+@(?:[a-zA-Z0-9-]+\\.)+[a-zA-Z]{2,4}");
}

#endif // APPINFO_H
