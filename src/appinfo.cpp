#include "appinfo.h"
#include <QDebug>
#include "version.h"

AppInfoClass::AppInfoClass()
{
    qDebug() << gGIT_VERSION;
}

const QString appName = "Crochet";
const QString appOrg = "Stitch Works Software";
