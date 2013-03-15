/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "teststitchset.h"

void TestStitchSet::initTestCase()
{
    mSet = new StitchSet();
}

void TestStitchSet::setupStitchSet()
{
    //TODO: make a couple of test sets of stitches.
    QVERIFY(mSet->stitchCount() == 0);
    mSet->loadXmlFile(":/crochet.xml");

    QVERIFY(mSet->stitchCount() == 109);
}

void TestStitchSet::findStitch()
{
    QFETCH(QString, name);
    QFETCH(QString, file);
    QFETCH(QString, desc);
    QFETCH(QString, cat);
    QFETCH(QString, ws);

    Stitch* s = mSet->findStitch(name);
    if(!s)
        qWarning() << "Couldn't find stitch in set: " << name;
    QVERIFY(s->name() == name);
    QVERIFY(s->file() == file);
    QVERIFY(s->description() == desc);
    QVERIFY(s->category() == cat);
    QVERIFY(s->wrongSide() == ws);
}

void TestStitchSet::findStitch_data()
{
    QTest::addColumn<QString>("name");
    QTest::addColumn<QString>("file");
    QTest::addColumn<QString>("desc");
    QTest::addColumn<QString>("cat");
    QTest::addColumn<QString>("ws");

    QTest::newRow("sl st") << "sl st" << "stitches/slip.svg" << "slip stitch" << "Basic" << "sl st";
    QTest::newRow("ch") << "ch" << "stitches/chain.svg" << "chain" << "Basic" << "ch";

}

void TestStitchSet::cleanupTestCase()
{
}

