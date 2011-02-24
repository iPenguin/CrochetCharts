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
    mSet->loadXmlStitchSet(":/crochet.xml");

    
    QVERIFY(mSet->stitchCount() == 10);
}

void TestStitchSet::findStitch()
{
    QFETCH(QString, name);
    QFETCH(QString, file);
    QFETCH(QString, desc);
    QFETCH(QString, cat);
    QFETCH(QString, ws);

    Stitch *s = mSet->findStitch(name);
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

    QTest::newRow("stitch 1") << "ss" << "stitches/slip.svg" << "slip stitch" << "Basic" << "ss";
    QTest::newRow("stitch 2") << "ch" << "stitches/chain.svg" << "chain" << "Basic" << "ch";

}

void TestStitchSet::cleanupTestCase()
{
}

