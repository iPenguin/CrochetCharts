/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
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
