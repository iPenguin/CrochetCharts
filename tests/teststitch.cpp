/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
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
    //FIXME: don't hard code the stitches.
    mS->setName("ch");
    mS->setFile("../stitches/ch.svg");
    mS->setDescription("chain");
    mS->setCategory("Basic");
    mS->setWrongSide("ch");

    QVERIFY(mS->name() == "ch");
    QVERIFY(mS->file() == "../stitches/ch.svg");
    QVERIFY(mS->description() == "chain");
    QVERIFY(mS->category() == "Basic");
    QVERIFY(mS->wrongSide() == "ch");
}

void TestStitch::stitchRender()
{
    QFETCH(QString, stitch);
    QFETCH(QString, stitchFile);
    QFETCH(qreal, width);
    QFETCH(qreal, height);
    QFETCH(QString, rasterHash);
    QFETCH(QString, svgHash);
    QFETCH(QString, svgHash2);

    mS->setFile(stitchFile);

    QString rasterImage = "teststitch-" + stitch + ".png";
    QString svgImage    = "teststitch-" + stitch + ".svg";
    QString svgImageAlt = "teststitch-" + stitch + "-alt.svg";

    //verify all images.
    QPixmap pix = QPixmap(width, height);
    pix.fill(QColor(Qt::white));
    QPainter pixp;
    pixp.begin(&pix);
    pixp.drawPixmap(0,0, *mS->renderPixmap());
    pixp.end();
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

    QPainter p;
    QSvgGenerator gen;
    gen.setFileName(svgImage);
    gen.setSize(QSize(width, height));
    gen.setViewBox(QRectF(0,0,width,height));

    p.begin(&gen);
    mS->renderSvg()->render(&p);
    p.end();

    f.setFileName(svgImage);

    if(!f.open(QIODevice::ReadOnly)) {
        QWARN("Could not compare svg file with expected results");
    }

    data = f.readAll();
    hash = QCryptographicHash::hash(data, QCryptographicHash::Sha1);

    hexHash = hash.toHex();
    QCOMPARE(hexHash, svgHash);
    f.flush();
    f.close();

    QPainter p2;
    QSvgGenerator gen2;

    gen2.setFileName(svgImageAlt);
    gen2.setSize(QSize(width, height));
    gen2.setViewBox(QRectF(0,0,width,height));

    p2.begin(&gen2);
    mS->renderSvg()->render(&p2);
    p2.end();

    f.setFileName(svgImageAlt);

    if(!f.open(QIODevice::ReadOnly)) {
        QWARN("Could not compare alt svg file with expected results");
    }

    data = f.readAll();
    hash = QCryptographicHash::hash(data, QCryptographicHash::Sha1);

    hexHash = hash.toHex();
    QCOMPARE(hexHash, svgHash2);
    f.flush();
    f.close();

    QVERIFY(mS->width() == width);
    QVERIFY(mS->height() == height);
}

void TestStitch::stitchRender_data()
{
    QTest::addColumn<QString>("stitch");
    QTest::addColumn<QString>("stitchFile");
    QTest::addColumn<qreal>("width");
    QTest::addColumn<qreal>("height");
    QTest::addColumn<QString>("rasterHash");
    QTest::addColumn<QString>("svgHash");
    QTest::addColumn<QString>("svgHash2");

    QTest::newRow("ch")     << "ch" << "../stitches/ch.svg" << 32.0 << 16.0
                            << "7343fe7fcfeb2cb816fab4276661c7c1e0348273"
                            << "2d34ba1749449edcee1709e6575cc60e1611e217"
                            << "78de9a725ebb724c4d5d7c66e6aeba520d9e4893";

    QTest::newRow("hdc")    << "hdc" << "../stitches/hdc.svg" << 32.0 << 64.0
                            << "e5a21b7e5a181d0f9f45a62334e67a32188c39c7"
                            << "4ad0f54e07e0c1ad960021689707658fbe9938ca"
                            << "dc1719c340182c8d0194f832a2a3e33e855176c4";
//TODO: render other stitches esp tall and wide stitches.
}

void TestStitch::cleanupTestCase()
{
}

