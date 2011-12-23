/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "testcell.h"
#include "../src/stitchlibrary.h"

#include <QPainter>
#include <QFile>
#include <QCryptographicHash>

void TestCell::initTestCase()
{
    scene = new QGraphicsScene();
    view = new QGraphicsView(scene);

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

    QVERIFY(mCell->sceneBoundingRect().width() == width);
    QVERIFY(mCell->sceneBoundingRect().height() == height);

}

void TestCell::setCellValues_data()
{

    QTest::addColumn<QString>("name");
    QTest::addColumn<qreal>("width");
    QTest::addColumn<qreal>("height");

    QTest::newRow("ch")    << "ch" << 32.0 << 16.0;
    QTest::newRow("hdc")   << "hdc" << 32.0 << 64.0;
    QTest::newRow("dc")    << "dc" << 32.0 << 80.0;

}

void TestCell::setRotation()
{
    QFETCH(QString, name);
    QFETCH(qreal, width);
    QFETCH(qreal, height);
    QFETCH(qreal, angle);
    QFETCH(QString, rasterHash);

    scene->addItem(mCell);

    Stitch* s = StitchLibrary::inst()->findStitch(name);
    mCell->setStitch(s);

    QPointF origin = QPointF(mCell->boundingRect().width()/2, mCell->boundingRect().height());
    mCell->setTransformOriginPoint(origin);
    mCell->setRotation(angle);

    QString rasterImage = "TestCell-setCellValues-" + name + ".png";

    QPixmap pix = QPixmap(width, height);
    pix.fill(QColor(Qt::white));
    QPainter p;
    p.begin(&pix);
    scene->render(&p);
    p.end();
    pix.save(rasterImage, "PNG", 100);

    QFile f(rasterImage);

    if(!f.open(QIODevice::ReadOnly)) {
        QFAIL("Could not compare png file with expected results");
    }

    QByteArray data = f.readAll();
    QByteArray hash = QCryptographicHash::hash(data, QCryptographicHash::Sha1);

    QString hexHash = hash.toHex();
    QCOMPARE(hexHash, rasterHash);
    f.flush();
    f.close();

    scene->removeItem(mCell);
}

void TestCell::setRotation_data()
{

    QTest::addColumn<QString>("name");
    QTest::addColumn<qreal>("width");
    QTest::addColumn<qreal>("height");
    QTest::addColumn<qreal>("angle");
    QTest::addColumn<QString>("rasterHash");

    QTest::newRow("ch")    << "ch" << 32.0 << 16.0 << 45.0 << "b7eb2d81f9c27b624c96102420f2465e22b6d05d";
    QTest::newRow("hdc")   << "hdc" << 32.0 << 64.0 << 45.0 << "ab6e9815f5cbaa93bc6ac6c233a2984463b21971";
    QTest::newRow("dc")    << "dc" << 32.0 << 80.0 << 45.0 << "ef2bd550192c0f280ecc300b206284779da3428e";

}

void TestCell::cleanupTestCase()
{
}
