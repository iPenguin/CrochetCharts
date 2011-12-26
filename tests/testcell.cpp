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
    i = 0;
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
    i++;

    Stitch* s = StitchLibrary::inst()->findStitch(name);
    mCell->setStitch(s);

    QPointF origin = QPointF(mCell->boundingRect().width()/2, mCell->boundingRect().height());
    mCell->setTransformOriginPoint(origin);
    mCell->setRotation(angle);

    QString rasterImage = "TestCell-SetRotation-" + name + "-" + QString::number(i) + ".png";

    saveScene(QSizeF(width, height), rasterImage);

    QString hexHash = hashFile(rasterImage);

    QCOMPARE(hexHash, rasterHash);

    mCell->setRotation(0);
    scene->removeItem(mCell);
}

void TestCell::setRotation_data()
{

    QTest::addColumn<QString>("name");
    QTest::addColumn<qreal>("width");
    QTest::addColumn<qreal>("height");
    QTest::addColumn<qreal>("angle");
    QTest::addColumn<QString>("rasterHash");

    QTest::newRow("ch")    << "ch" << 32.0 << 16.0 << 45.0 << "3acb54f2d0d610851417e9a15fa5bcf56c5ca934";
    QTest::newRow("hdc")   << "hdc" << 32.0 << 64.0 << 45.0 << "8810b0554ae658d526d742512b8fa6b4c9c5039d";
    QTest::newRow("dc")    << "dc" << 32.0 << 80.0 << 45.0 << "525c69657503c470995b73633c096b185f0390bc";

}

void TestCell::setScale()
{

    QFETCH(QString, name);
    QFETCH(qreal, width);
    QFETCH(qreal, height);
    QFETCH(qreal, scaleX);
    QFETCH(qreal, scaleY);
    QFETCH(QString, rasterHash);

    scene->addItem(mCell);
    i++;

    Stitch* s = StitchLibrary::inst()->findStitch(name);
    mCell->setStitch(s);

    QPointF origin = QPointF(mCell->boundingRect().width()/2, mCell->boundingRect().height());
    mCell->setTransformOriginPoint(origin);
    mCell->setScale(scaleX, scaleY);

    QString rasterImage = "TestCell-ScaleTest-" + name + "-" + QString::number(i) + ".png";

    saveScene(QSizeF(width, height), rasterImage);

    QString hexHash = hashFile(rasterImage);

    QCOMPARE(hexHash, rasterHash);

    mCell->setScale(1.0, 1.0);
    scene->removeItem(mCell);

}

void TestCell::setScale_data()
{

    QTest::addColumn<QString>("name");
    QTest::addColumn<qreal>("width");
    QTest::addColumn<qreal>("height");
    QTest::addColumn<qreal>("scaleX");
    QTest::addColumn<qreal>("scaleY");
    QTest::addColumn<QString>("rasterHash");

    QTest::newRow("ch")    << "ch" << 32.0 << 16.0 << 1.0 << 1.0 << "97999ab9dd7ae38d62026c72e26dbae3757bf8ae";
    QTest::newRow("hdc")   << "hdc" << 32.0 << 64.0 << 2.5 << 2.5 << "4986b9226e512c9ac05108187d66c8352501a298";
    QTest::newRow("dc")    << "dc" << 32.0 << 80.0 << 2.5 << 2.5 << "e2d1d603695e924a124ff0685be98153872da642";
}

void TestCell::setBgColor()
{

    QFETCH(QString, name);
    QFETCH(qreal, width);
    QFETCH(qreal, height);
    QFETCH(QString, color);
    QFETCH(QString, rasterHash);

    scene->addItem(mCell);
    i++;

    Stitch* s = StitchLibrary::inst()->findStitch(name);
    mCell->setStitch(s);

    QPointF origin = QPointF(mCell->boundingRect().width()/2, mCell->boundingRect().height());
    mCell->setTransformOriginPoint(origin);
    mCell->setColor(QColor(color));

    QString rasterImage = "TestCell-BgColor-" + name + "-" + QString::number(i) + ".png";

    saveScene(QSizeF(width, height), rasterImage);

    QString hexHash = hashFile(rasterImage);

    QCOMPARE(hexHash, rasterHash);

    mCell->setColor();
    scene->removeItem(mCell);
}

void TestCell::setBgColor_data()
{

    QTest::addColumn<QString>("name");
    QTest::addColumn<qreal>("width");
    QTest::addColumn<qreal>("height");
    QTest::addColumn<QString>("color");
    QTest::addColumn<QString>("rasterHash");

    QTest::newRow("ch")    << "ch" << 32.0 << 16.0 << "#0000FF" << "9784f1d2220b56081cc3292fe7c9fb215d72a736";
    QTest::newRow("hdc")   << "hdc" << 32.0 << 64.0 << "#FF0000" << "4664a9531fd5d365e2abec8be1d767f716b74412";
    QTest::newRow("dc")    << "dc" << 32.0 << 80.0 << "#00FF00" << "16fb469cbc74011c59c4a8363af7eac50447197e";

}

void TestCell::saveScene(QSizeF size, QString fileName)
{

    QPixmap pix = QPixmap(scene->sceneRect().width(), scene->sceneRect().height());
    pix.fill(QColor(Qt::white));
    QPainter p;
    p.begin(&pix);
    scene->render(&p);
    p.end();
    pix.save(fileName, "PNG", 100);

}

QString TestCell::hashFile(QString fileName)
{
    QFile f(fileName);

    if(!f.open(QIODevice::ReadOnly)) {
        qFatal("Could not compare png file with expected results");
    }

    QByteArray data = f.readAll();
    QByteArray hash = QCryptographicHash::hash(data, QCryptographicHash::Sha1);

    QString hexHash = hash.toHex();
    f.flush();
    f.close();

    return hexHash;
}

void TestCell::cleanupTestCase()
{
}
