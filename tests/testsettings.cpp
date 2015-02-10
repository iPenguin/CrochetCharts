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
#include "testsettings.h"

void TestSettings::initTestCase()
{
    qDebug() << Settings::inst()->fileName();
}



void TestSettings::setSettings()
{

    QFETCH(QString, key);
    QFETCH(QVariant, value);
    QFETCH(QVariant, value2);

    QVariant test = Settings::inst()->value(key);

    QVERIFY(test == value);

    Settings::inst()->setValue(key, value2);
    QVariant test2 = Settings::inst()->value(key);

    QVERIFY(test2 == value2);

}

void TestSettings::setSettings_data()
{
    QTest::addColumn<QString>("key");
    QTest::addColumn<QVariant>("value");
    QTest::addColumn<QVariant>("value2");

    QTest::newRow("check for updates") << "checkForUpdates" << QVariant(true) << QVariant(false);
    QTest::newRow("max files") << "maxRecentFiles" << QVariant(5) << QVariant(10);
    QTest::newRow("default stitch") << "defaultStitch" << QVariant("ch") << QVariant("dc");

}

void TestSettings::cleanupTestCase()
{
}
