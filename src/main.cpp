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

#include "stitchcollection.h"

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

    splash.showMessage(QObject::tr("Loading Stitch Sets..."));
    StitchCollection *collection = StitchCollection::inst();
    collection->loadStitchSets();
    splash.showMessage(QObject::tr("Populating Master Stitch Set..."));
    collection->populateMasterSet();

    splash.showMessage(QObject::tr("Loading Main Window..."));

    //TODO: pass the filename in from the command line to the loading application.
    //foreach file name create a new MainWindow. Make sure it works on the Mac!!
    MainWindow w;
    w.show();
    splash.finish(&w);
    return a.exec();
}
