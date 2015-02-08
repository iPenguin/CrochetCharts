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
#ifndef APPINFO_H
#define APPINFO_H

#include "version.h"
#include <QString>
#include <QRegExp>
#include <qobject.h>

/**
 * singleton class to supply general application-wide info to all classes.
 */

class AppInfo
{
public:
    static AppInfo* inst()
    {
        if (!mInstance)   // Only allow one instance of the settings.
      mInstance = new AppInfo();
      return mInstance;
    }
    
    const QString appName;
    const QString appOrg;
    
    const QString appOrgDomain;
    const QString appOrgContact;
    
    const QString appVersion;
    const QString appVersionShort;
    const QString appBuildInfo;
    
    const QRegExp emailRegExp;
    
    const QString liveUpdatePage;
    const QString liveUpdatePageVals;

    const QString demoString;
    
    const QString projectLife;

    const quint32 magicNumber;
    const quint32 magicNumberSet;
    
private:
    static AppInfo* mInstance;
    
    AppInfo();
    
};

#endif // APPINFO_H
