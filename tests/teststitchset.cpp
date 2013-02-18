/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "teststitchset.h"

#include "tests.h"

void TestStitchSet::initTestCase()
{
    mSet = new StitchSet();
}

void TestStitchSet::setupStitchSet()
{
    //TODO: make a couple of test sets of stitches.
    QVERIFY(mSet->stitchCount() == 0);
    mSet->loadXmlFile("../crochet.xml");

    QVERIFY(mSet->stitchCount() == 109);
}

void TestStitchSet::findStitch()
{
    QFETCH(QString, uid);
    QFETCH(QString, name);
    QFETCH(bool, exists);
    QFETCH(QString, file);
    QFETCH(QString, desc);
    QFETCH(QString, cat);
    QFETCH(QString, ws);

    Stitch* s = mSet->findStitch(uid);

    if(!s) {
        if(exists) {
            qDebug() << name;
            QFAIL("stitch doesn't exist and it should: ");
        } else {
            QCOMPARE((bool)s, exists);
            return;
        }
    }
        
    QVERIFY(s->uid() == uid);
    QVERIFY(s->name() == name);
    QVERIFY(s->file() == file);
    QVERIFY(s->description() == desc);
    QVERIFY(s->category() == cat);
    QVERIFY(s->wrongSide() == ws);
}

void TestStitchSet::findStitch_data()
{
    QTest::addColumn<QString>("uid");
    QTest::addColumn<QString>("name");
    QTest::addColumn<bool>("exists");
    QTest::addColumn<QString>("file");
    QTest::addColumn<QString>("desc");
    QTest::addColumn<QString>("cat");
    QTest::addColumn<QString>("ws");

    QTest::newRow("sl st") << _SL_ST_ << "sl st" << true << ":/stitches/sl_st.svg"
                           << "slip stitch" << "Default" << _SL_ST_;
    QTest::newRow("ch") << _CH_ << "ch" << true << ":/stitches/ch.svg"
                        << "chain" << "Default" << _CH_;
    QTest::newRow("hdc") << _HDC_ << "hdc" << true << ":/stitches/hdc.svg"
                         << "half double crochet" << "Default" << _HDC_;
    QTest::newRow("5-dc shell") << _5_DC_SHELL_ << "5-dc shell" << true << ":/stitches/5dc_shell.svg"
                                    << "5 double crochet shell" << "Default" << _5_DC_SHELL_;
    QTest::newRow("stitch dne") << "sha1" << "bcm" << false << "" << "" << "" << "";

}

void TestStitchSet::saveLoadDataSet()
{
    QString fileName = "teststitchset-savefile.set";
    mSet->saveDataFile(fileName);

    StitchSet* testSet = new StitchSet();
    testSet->loadDataFile(fileName, "set1.xml");

    QVERIFY(testSet->stitchCount() == mSet->stitchCount());

    foreach(Stitch* s, mSet->stitches()) {
        Stitch* testSt = testSet->findStitch(s->uid());
        if(!testSt)
            QFAIL("Could not find stitch.");

        QVERIFY(s->uid() == testSt->uid());
        QVERIFY(s->name() == testSt->name());
        QVERIFY(s->category() == testSt->category());
        QVERIFY(s->description() == testSt->description());
        QVERIFY(s->file() == testSt->file());
        QVERIFY(s->height() == testSt->height());
        QVERIFY(s->width() == testSt->width());
        QVERIFY(s->isSvg() == testSt->isSvg());
    }
    
}

void TestStitchSet::cleanupTestCase()
{
}

