#include "teststitch.h"

void TestStitch::initTestCase()
{
    qDebug() << "Begin all tests";
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

}

void TestStitch::cleanupTestCase()
{
    qDebug() << "End all tests";
}

