/*************************************************\
| (c) 2010-2011 Stitch Works Software             |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "teststitch.h"

#include <QPainter>
#include <QPixmap>
#include <QtSvg/QSvgRenderer>
#include <QtSvg/QSvgGenerator>

#include <QFile>
#include <QCryptographicHash>
#include <QDebug>

void TestStitch::initTestCase()
{
    mS = new Stitch();
}

void TestStitch::stitchSetup()
{
    mS->setName("ch");
    mS->setFile(":/stitches/chain.svg");
    mS->setDescription("chain");
    mS->setCategory("Basic");
    mS->setWrongSide("ch");

    QVERIFY(mS->name() == "ch");
    QVERIFY(mS->file() == ":/stitches/chain.svg");
    QVERIFY(mS->description() == "chain");
    QVERIFY(mS->category() == "Basic");
    QVERIFY(mS->wrongSide() == "ch");
}

void TestStitch::stitchRender()
{
    QFETCH(int, width);
    QFETCH(int, height);
    QFETCH(QString, rasterHash);
    QFETCH(QString, svgHash);

    QString rasterImage = "teststitch-render.jpg";
    QString svgImage    = "teststitch-render.svg";

    //verify all images.
    QPixmap pix = QPixmap(width, height);
    pix.fill(QColor(Qt::white));
    QPainter pixp;
    pixp.begin(&pix);
    pixp.drawPixmap(0,0, *mS->renderPixmap());
    pixp.end();
    pix.save(rasterImage, "", 100);

    QFile f(rasterImage);

    if(!f.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not compare raster file with expected results";
        return;
    }
    QByteArray data = f.readAll();
    QByteArray hash = QCryptographicHash::hash(data, QCryptographicHash::Sha1);

    QString hexHash = hash.toHex();
    QCOMPARE(hexHash, rasterHash);
    f.flush();
    f.close();

    QPainter p;
    QSvgGenerator gen;
    gen.setFileName("teststitch-render.svg");
    gen.setSize(QSize(32, 64));
    gen.setResolution(100);
    gen.setViewBox(QRectF(0,0,32,64));
    p.begin(&gen);
    mS->renderSvg()->render(&p);
    p.end();

    f.setFileName(svgImage);

    if(!f.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not compare svg file with expected results";
        return;
    }
    data = f.readAll();
    hash = QCryptographicHash::hash(data, QCryptographicHash::Sha1);

    hexHash = hash.toHex();
    QCOMPARE(hexHash, svgHash);
    f.flush();
    f.close();

}

void TestStitch::stitchRender_data()
{
    QTest::addColumn<int>("width");
    QTest::addColumn<int>("height");
    QTest::addColumn<QString>("rasterHash");
    QTest::addColumn<QString>("svgHash");

    QTest::newRow("1st")   << (int)32 << (int)64 << "786ff038d0ada0c449a19007892bd1b443a558f3" << "8a64346c61917cc5d40a113710e6f82f75fbff86";
    QTest::newRow("2nd")   << (int)32 << (int)64 << "786ff038d0ada0c449a19007892bd1b443a558f3" << "8a64346c61917cc5d40a113710e6f82f75fbff86";
//TODO: render other stitches esp tall and wide stitches.
}

void TestStitch::cleanupTestCase()
{
}

