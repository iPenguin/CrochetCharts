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

#include "appinfo.h"
#include <QFileOpenEvent>

#include "stitchlibrary.h"

QStringList Application::mFileOpenEventList;

Application::Application(int &argc, char **argv)
    : QApplication(argc, argv),
      mW(0)
{
    qApp->setApplicationName(AppInfo::inst()->appName);
    qApp->setApplicationVersion(AppInfo::inst()->appVersion);
    qApp->setOrganizationName(AppInfo::inst()->appOrg);
    qApp->setOrganizationDomain(AppInfo::inst()->appOrgDomain);

    StitchLibrary* library = StitchLibrary::inst();
    library->loadStitchSets();
}

bool Application::event(QEvent *event)
{
    switch (event->type()) {
        case QEvent::FileOpen: {
            QString file = static_cast<QFileOpenEvent *>(event)->file();
            loadFile(file);
            return true;
        }
        default:{
            return QApplication::event(event);
        }
    }
}

void Application::loadFile(const QString &fileName)
{
    if(!mW) {
        qWarning() << "Application::loadFile cannot find a window to load this file!";
        return;
    }

    mW->loadFile(fileName);
}

void Application::resendFileOpenEvents(QObject *receiver)
{

    if(mFileOpenEventList.count() > 0)
    {

        QStringListIterator i(mFileOpenEventList);
        while(i.hasNext())
        {
            QFileOpenEvent event(i.next());
            Application::sendEvent(receiver, &event);
        }
        mFileOpenEventList.clear();
    }
}

void Application::setMainWindow(MainWindow *window)
{
    mW = window;
}
