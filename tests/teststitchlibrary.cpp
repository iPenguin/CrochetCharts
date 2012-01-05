/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "teststitchlibrary.h"
#include "stitchset.h"

#include "tests.h"

#include <QDebug>

void TestStitchLibrary::initTestCase()
{
    StitchLibrary::inst()->loadStitchSets();
}

void TestStitchLibrary::findStitch()
{

    QFETCH(QString, uid);
    QFETCH(QString, name);
    QFETCH(bool, exists);
    QFETCH(QString, file);
    QFETCH(QString, desc);
    QFETCH(QString, cat);
    QFETCH(QString, ws);
    
    Stitch* s = StitchLibrary::inst()->findStitch(uid);

    if(!s) {
        if(exists) {
            QFAIL("stitch not found in set as it should have been");
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

void TestStitchLibrary::findStitch_data()
{
    
    QTest::addColumn<QString>("uid");
    QTest::addColumn<QString>("name");
    QTest::addColumn<bool>("exists");
    QTest::addColumn<QString>("file");
    QTest::addColumn<QString>("desc");
    QTest::addColumn<QString>("cat");
    QTest::addColumn<QString>("ws");

    QTest::newRow("sl st") << _SL_ST_ << "sl st" << true << ":/stitches/sl_st.svg" << "slip stitch" << "Default" << _SL_ST_;
    QTest::newRow("ch") << _CH_ << "ch" << true << ":/stitches/ch.svg" << "chain" << "Default" << _CH_;
    QTest::newRow("hdc") << _HDC_ << "hdc" << true << ":/stitches/hdc.svg" << "half double crochet" << "Default" << _HDC_;
    QTest::newRow("5-dc shell") << _5_DC_SHELL_ << "5-dc shell" << true << ":/stitches/5dc_shell.svg"
                                    << "5 double crochet shell" << "Default" << _5_DC_SHELL_;
    QTest::newRow("stitch dne") << "sha1" << "bcm" << false << "" << "" << "" << "";
    
}

void TestStitchLibrary::cleanupTestCase()
{
    
}
