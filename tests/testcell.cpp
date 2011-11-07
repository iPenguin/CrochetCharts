/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "testcell.h"
#include "../src/stitchlibrary.h"

void TestCell::initTestCase()
{
    StitchLibrary::inst()->loadStitchSets();
    mCell = new Cell();
}

void TestCell::setCellValues()
{
    QFETCH(QString, name);
    QFETCH(qreal, width);
    QFETCH(qreal, height);

    Stitch* s = StitchLibrary::inst()->findStitch(name);
    mCell->setStitch(s);


}

void TestCell::setCellValues_data()
{
    QTest::addColumn<QString>("name");
    QTest::addColumn<qreal>("width");
    QTest::addColumn<qreal>("height");

    QTest::newRow("ch")    << "ch" << 32.0 << 16.0;
    QTest::newRow("hdc")   << "hdc" << 32.0 << 64.0;

}

void TestCell::cleanupTestCase()
{
}
