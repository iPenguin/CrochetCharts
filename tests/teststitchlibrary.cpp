/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "teststitchlibrary.h"
#include "stitchset.h"

#include <QDebug>

void TestStitchLibrary::initTestCase()
{
    StitchLibrary::inst()->loadStitchSets();
}

void TestStitchLibrary::findStitch()
{

    QFETCH(QString, name);
    QFETCH(bool, exists);
    QFETCH(QString, file);
    QFETCH(QString, desc);
    QFETCH(QString, cat);
    QFETCH(QString, ws);
    
    Stitch* s = StitchLibrary::inst()->findStitch(name);

    if(!s) {
        if(exists) {
            QFAIL("stitch not found in set as it should have been");
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

void TestStitchLibrary::findStitch_data()
{
    
    QTest::addColumn<QString>("name");
    QTest::addColumn<bool>("exists");
    QTest::addColumn<QString>("file");
    QTest::addColumn<QString>("desc");
    QTest::addColumn<QString>("cat");
    QTest::addColumn<QString>("ws");

    QTest::newRow("sl st") << "sl st" << true << ":/stitches/sl_st.svg" << "slip stitch" << "Default" << "sl st";
    QTest::newRow("ch") << "ch" << true << ":/stitches/ch.svg" << "chain" << "Default" << "ch";
    QTest::newRow("hdc") << "hdc" << true << ":/stitches/hdc.svg" << "half double crochet" << "Default" << "hdc";
    QTest::newRow("5-dc shell") << "5-dc shell" << true << ":/stitches/5dc_shell.svg"
                                    << "5 double crochet shell" << "Default" << "5-dc shell";
    QTest::newRow("stitch dne") << "bcm" << false << "" << "" << "" << "";
    
}

void TestStitchLibrary::cleanupTestCase()
{
    
}
