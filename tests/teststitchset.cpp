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
    mSet->loadXmlFile("../crochet.xml");

    QVERIFY(mSet->stitchCount() == 24);
}

void TestStitchSet::findStitch()
{
    QFETCH(QString, name);
    QFETCH(bool, exists);
    QFETCH(QString, file);
    QFETCH(QString, desc);
    QFETCH(QString, cat);
    QFETCH(QString, ws);

    Stitch* s = mSet->findStitch(name);

    if(!s) {
        if(exists) {
            QFAIL("stitch doesn't exist and it should");
        } else {
            QCOMPARE((bool)s, exists);
            return;
        }
    }
        
    QVERIFY(s->name() == name);
    QVERIFY(s->file() == file);
    QVERIFY(s->description() == desc);
    QVERIFY(s->category() == cat);
    QVERIFY(s->wrongSide() == ws);
}

void TestStitchSet::findStitch_data()
{
    QTest::addColumn<QString>("name");
    QTest::addColumn<bool>("exists");
    QTest::addColumn<QString>("file");
    QTest::addColumn<QString>("desc");
    QTest::addColumn<QString>("cat");
    QTest::addColumn<QString>("ws");

    QTest::newRow("stitch 1") << "sl st" << true << ":/stitches/sl_st.svg" << "slip stitch" << "Basic" << "sl st";
    QTest::newRow("stitch 2") << "ch" << true << ":/stitches/ch.svg" << "chain" << "Basic" << "ch";
    QTest::newRow("stitch dne") << "bcm" << false << "" << "" << "" << "";

}

void TestStitchSet::cleanupTestCase()
{
}

