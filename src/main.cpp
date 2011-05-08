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
    
    qApp->setApplicationName(AppInfo::inst()->appName);
    qApp->setApplicationVersion(AppInfo::inst()->appVersion);
    qApp->setOrganizationName(AppInfo::inst()->appOrg);
    qApp->setOrganizationDomain(AppInfo::inst()->appOrgDomain);

    Q_INIT_RESOURCE(crochet);

    SplashScreen splash;
    splash.show();
    splash.showMessage(QObject::tr("Loading..."));
    qApp->processEvents();
   
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

    QStringList arguments = QCoreApplication::arguments();
    arguments.removeFirst(); // remove the application name from the list.
    
    MainWindow w(arguments);
    w.show();
    //splash.finish(&w);
    return a.exec();
}
