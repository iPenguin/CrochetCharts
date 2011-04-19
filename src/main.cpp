/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include <QtGui/QApplication>
#include "mainwindow.h"
#include "appinfo.h"

#include <QDebug>

#include "license.h"
#include "licensewizard.h"
#include "settings.h"

#include "stitchlibrary.h"

#include "splashscreen.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    qApp->setApplicationName(AppInfo::appName);
    qApp->setApplicationVersion(AppInfo::appVersion);
    qApp->setOrganizationName(AppInfo::appOrg);
    qApp->setOrganizationDomain(AppInfo::appOrgDomain);

    Q_INIT_RESOURCE(crochet);

    SplashScreen splash;
    splash.show();
    splash.showMessage(QObject::tr("Loading..."));
    qApp->processEvents();


    AppInfoClass *apc = new AppInfoClass;
   
    QString userSn = Settings::inst()->value("serialNumber").toString();
    QString userLicense = Settings::inst()->value("license").toString();
    QString userEmail = Settings::inst()->value("email").toString();

    if(!License::isValidLicense(userLicense, userSn, userEmail)) {
        splash.showMessage(QObject::tr("Loading: License Wizard"));
        LicenseWizard wizard;
        splash.hide();
        if(wizard.exec() != QWizard::Accepted)
                return -1;
        splash.show();
    }

    splash.showMessage(QObject::tr("Loading All Stitch Sets..."));
    StitchLibrary *collection = StitchLibrary::inst();
    collection->loadStitchSets();
    splash.showMessage(QObject::tr("Populating Master Stitch Set..."));
    collection->setupMasterSet();

    splash.showMessage(QObject::tr("Loading Main Window..."));

    MainWindow w;
    w.show();
    splash.finish(&w);
    return a.exec();
}
