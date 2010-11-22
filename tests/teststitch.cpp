#include "teststitch.h"

void Stitch::initTestCase()
{
    qDebug() << "Begin all tests";
    s = new Stitch();
}

void Stitch::stitchSetup()
{
    s->setName("ch");
    s->setFile(":/stitches/chain.svg");
    s->setDescription("chain");
    s->setCategory("Basic");
    s->setWrongSide("ch");

    QVARIFY(s->name() == "ch");
    QVARIFY(s->file() == ":/stitches/chain.svg");
    QVARIFY(s->description() == "chain");
    QVARIFY(s->category() == "Basic");
    QVARIFY(s->wrongSide() == "ch");
}

void Stitch::stitchRender()
{
    s->setRotation(90); //rotation around the center point.
    s->setAngle(15); //angle from the base of the stitch.


    //FIXME: finish this function
    // sha1 hash of the exported image and svg compaired to the expected output.

    //QPixmap p = s->renderImage();
    //QSvg svg = s->renderSvg();

    //change rotation
    //render image
    //change angle
    //render image

    //verify all images.

}

void Stitch::cleanupTestCase()
{
    qDebug() << "End all tests";
}

