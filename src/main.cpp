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
#include "updatefunctions.h"

#include "errorhandler.h"

int main(int argc, char *argv[])
{
    qInstallMsgHandler(errorHandler);
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

#ifndef APPLE_APP_STORE
    if(!License::isValidLicense(userLicense, userSn, userEmail)) {
        splash.showMessage(QObject::tr("Loading: License Wizard"));
        LicenseWizard wizard;
        splash.hide();
        if(wizard.exec() != QWizard::Accepted)
                return -1;
        Settings::inst()->saveSettings();
        splash.show();
    }
#endif //APPLE_APP_STORE

    QString curVersion = AppInfo::inst()->appVersion;
    QString lastUsed = Settings::inst()->value("lastUsed").toString();
    updateFunction(lastUsed);
    Settings::inst()->setValue("version", curVersion);

    splash.showMessage(QObject::tr("Loading Stitches..."));
    StitchLibrary* library = StitchLibrary::inst();
    library->loadStitchSets();

    splash.showMessage(QObject::tr("Loading Ui..."));

    QStringList arguments = QCoreApplication::arguments();
    arguments.removeFirst(); // remove the application name from the list.
    
    MainWindow w(arguments);
    w.showMaximized();
    splash.finish(&w);
    return a.exec();
}
