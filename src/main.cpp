#include <QtGui/QApplication>
#include "mainwindow.h"
#include "appinfo.h"

#include <QDebug>

#include "license.h"
#include "licensewizard.h"
#include "settings.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qApp->setApplicationName(AppInfo::appName);
    qApp->setApplicationVersion(AppInfo::appVersion);
    qApp->setOrganizationName(AppInfo::appOrg);
    qApp->setOrganizationDomain(AppInfo::appOrgDomain);

    Q_INIT_RESOURCE(crochet);

    QString userSn = Settings::instance()->value("serialNumber", QVariant("0000-000-0000")).toString();

    if(!License::isValid(userSn)) {
        LicenseWizard wizard;
        if(wizard.exec() != QWizard::Accepted)
                return -1;
    }

    MainWindow w;
    w.show();

    return a.exec();
}
