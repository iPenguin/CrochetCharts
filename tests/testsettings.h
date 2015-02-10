/****************************************************************************\
 Copyright (c) 2011-2014 Stitch Works Software
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
#ifndef TESTSETTINGS_H
#define TESTSETTINGS_H

#include <QtTest/QTest>
#include <QDebug>
#include <QObject>

#include "../src/settings.h"
#include "../src/appinfo.h"

class TestSettings : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void setSettings();
    void setSettings_data();
    void cleanupTestCase();

private:

};
#endif //TESTSETTINGS_H
