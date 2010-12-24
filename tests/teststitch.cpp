#include "teststitch.h"

#include <QPainter>
#include <QPixmap>
#include <QtSvg/QSvgRenderer>
#include <QtSvg/QSvgGenerator>

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
    mS->setRotation(90); //rotation around the center point.
    mS->setAngle(15); //angle from the base of the stitch.


    //FIXME: finish this function
    // sha1 hash of the exported image and svg compaired to the expected output.

    //QPixmap p = mS->renderImage();
    //QSvg svg = mS->renderSvg();

    //change rotation
    //render image
    //change angle
    //render image

    //verify all images.
    QPixmap pix = QPixmap(32, 64);
    pix.fill(QColor(Qt::white));
    QPainter pixp;
    pixp.begin(&pix);
    pixp.drawPixmap(0,0, *mS->renderPixmap());
    pixp.end();
    pix.save("teststitch-render.jpg", "", 100);

    QPainter p;
    QSvgGenerator gen;
    gen.setFileName("teststitch-render.svg");
    gen.setSize(QSize(32, 64));
    gen.setResolution(100);
    gen.setViewBox(QRectF(0,0,32,64));
    p.begin(&gen);
    mS->renderSvg()->render(&p);
    p.end();

}

void TestStitch::cleanupTestCase()
{
}

