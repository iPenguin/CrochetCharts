#include <QtGui/QApplication>
#include "mainwindow.h"
#include "appinfo.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qApp->setApplicationName(AppInfo::appName);
    qApp->setApplicationVersion(AppInfo::appVersion);
    qApp->setOrganizationName(AppInfo::appOrg);
    qApp->setOrganizationDomain(AppInfo::appOrgDomain);


    MainWindow w;
    w.show();

    return a.exec();
}
