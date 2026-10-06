/****************************************************************************\
 Copyright (c) 2010-2014 Stitch Works Software
 Brian C. Milco <bcmilco@gmail.com>

 This file is part of Crochet Charts.

 Crochet Charts is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Crochet Charts is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Crochet Charts. If not, see <http://www.gnu.org/licenses/>.

 \****************************************************************************/
#include "application.h"
#include "mainwindow.h"
#include "appinfo.h"

#include <QDebug>
#include <QDir>

#include "settings.h"

#include "splashscreen.h"
#include "updatefunctions.h"

#include "errorhandler.h"

int main(int argc, char *argv[])
{
    qInstallMessageHandler(errorHandler);
    Application a(argc, argv);

    QStringList arguments = QCoreApplication::arguments();
    arguments.removeFirst(); // remove the application name from the list.

    MainWindow w(arguments);
    a.setMainWindow(&w);

    Q_INIT_RESOURCE(crochet);

    SplashScreen splash;
    splash.show();
    splash.showMessage(QObject::tr("Loading..."));
    qApp->processEvents();
   
    QString curVersion = AppInfo::inst()->appVersion;
    QString lastUsed = Settings::inst()->value("lastUsed").toString();
    updateFunction(lastUsed);
    Settings::inst()->setValue("version", curVersion);

    splash.showMessage(QObject::tr("Loading Ui..."));

    w.showMaximized();
    splash.finish(&w);
    return a.exec();
}
