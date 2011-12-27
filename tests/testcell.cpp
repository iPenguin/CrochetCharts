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

    StitchLibrary::inst()->loadStitchSets();
}

void TestCell::setCellValues()
{
    QFETCH(QString, name);
    QFETCH(qreal, width);
    QFETCH(qreal, height);

    Cell* c = new Cell();
    Stitch* s = StitchLibrary::inst()->findStitch(name);
    c->setStitch(s);

    QVERIFY(c->sceneBoundingRect().width() == width);
    QVERIFY(c->sceneBoundingRect().height() == height);

    delete c;
    c = 0;

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

    Cell* c = new Cell();
    QGraphicsScene *scene = new QGraphicsScene();
    scene->addItem(c);
    i++;

    Stitch* s = StitchLibrary::inst()->findStitch(name);
    c->setStitch(s);

    QPointF origin = QPointF(c->boundingRect().width()/2, c->boundingRect().height());
    c->setTransformOriginPoint(origin);
    c->setRotation(angle);

    QString rasterImage = "TestCell-SetRotation-" + QString::number(i) + "-" + name + ".png";

    saveScene(scene, QSizeF(width, height), rasterImage);

    QString hexHash = hashFile(rasterImage);

    QCOMPARE(hexHash, rasterHash);

    scene->removeItem(c);
    delete c;
    c = 0;
}

void TestCell::setRotation_data()
{

    QTest::addColumn<QString>("name");
    QTest::addColumn<qreal>("width");
    QTest::addColumn<qreal>("height");
    QTest::addColumn<qreal>("angle");
    QTest::addColumn<QString>("rasterHash");

    QTest::newRow("ch")    << "ch" << 32.0 << 16.0 << 45.0 << "3acb54f2d0d610851417e9a15fa5bcf56c5ca934";
    QTest::newRow("hdc")   << "hdc" << 32.0 << 64.0 << 45.0 << "a1d58264e09bfe2583a4053ebb9753332c1be217";
    QTest::newRow("dc")    << "dc" << 32.0 << 80.0 << 45.0 << "b76157343c5e9b71b519bfba0d013275edfa8d98";

}

void TestCell::setScale()
{

    QFETCH(QString, name);
    QFETCH(qreal, width);
    QFETCH(qreal, height);
    QFETCH(qreal, scaleX);
    QFETCH(qreal, scaleY);
    QFETCH(QString, rasterHash);

    Cell* c = new Cell();
    QGraphicsScene *scene = new QGraphicsScene();

    scene->addItem(c);
    i++;

    Stitch* s = StitchLibrary::inst()->findStitch(name);
    c->setStitch(s);

    QPointF origin = QPointF(c->boundingRect().width()/2, c->boundingRect().height());
    c->setTransformOriginPoint(origin);
    c->setScale(scaleX, scaleY);

    QString rasterImage = "TestCell-ScaleTest-" + QString::number(i) + "-" + name + ".png";

    saveScene(scene, QSizeF(width, height), rasterImage);

    QString hexHash = hashFile(rasterImage);

    QCOMPARE(hexHash, rasterHash);

    scene->removeItem(c);
    delete c;
    c = 0;
}

void TestCell::setScale_data()
{

    QTest::addColumn<QString>("name");
    QTest::addColumn<qreal>("width");
    QTest::addColumn<qreal>("height");
    QTest::addColumn<qreal>("scaleX");
    QTest::addColumn<qreal>("scaleY");
    QTest::addColumn<QString>("rasterHash");

    QTest::newRow("ch")    << "ch" << 32.0 << 16.0 << 1.0 << 1.0 << "0be6578d22717ba56082a408e8ed128e8114a30c";
    QTest::newRow("hdc")   << "hdc" << 32.0 << 64.0 << 2.5 << 2.5 << "090bf8ac828e1d2ad0da3ec240539276148bc134";
    QTest::newRow("dc")    << "dc" << 32.0 << 80.0 << 2.5 << 2.5 << "bdb70eea2145b79c7cb4e50a6148f5ec4d09f708";
}

void TestCell::setBgColor()
{

    QFETCH(QString, name);
    QFETCH(qreal, width);
    QFETCH(qreal, height);
    QFETCH(QString, color);
    QFETCH(QString, rasterHash);

    QGraphicsScene *scene = new QGraphicsScene();
    Cell* c = new Cell();
    scene->addItem(c);
    i++;

    Stitch* s = StitchLibrary::inst()->findStitch(name);
    c->setStitch(s);

    QPointF origin = QPointF(c->boundingRect().width()/2, c->boundingRect().height());
    c->setTransformOriginPoint(origin);
    c->setColor(QColor(color));

    QString rasterImage = "TestCell-BgColor-" + QString::number(i) + "-" + name + ".png";

    saveScene(scene, QSizeF(width, height), rasterImage);

    QString hexHash = hashFile(rasterImage);

    QCOMPARE(hexHash, rasterHash);

    scene->removeItem(c);
    delete c;
    c = 0;
}

void TestCell::setBgColor_data()
{

    QTest::addColumn<QString>("name");
    QTest::addColumn<qreal>("width");
    QTest::addColumn<qreal>("height");
    QTest::addColumn<QString>("color");
    QTest::addColumn<QString>("rasterHash");

    QTest::newRow("ch")    << "ch" << 32.0 << 16.0 << "#0000FF" << "9d6c0ac6e51809f662573c9432c0b5c7064c2021";
    QTest::newRow("hdc")   << "hdc" << 32.0 << 64.0 << "#FF0000" << "c3105395f5d7e6ad8269208d25e8be76ab7a9455";
    QTest::newRow("dc")    << "dc" << 32.0 << 80.0 << "#00FF00" << "0a4fedb0079e244671ab5a376984f40df3394b94";

}

void TestCell::setAllProperties()
{

    QFETCH(QString, name);
    QFETCH(qreal, width);
    QFETCH(qreal, height);
    QFETCH(qreal, angle);
    QFETCH(qreal, scaleX);
    QFETCH(qreal, scaleY);
    QFETCH(QString, color);
    QFETCH(QString, rasterHash);

    QGraphicsScene *scene = new QGraphicsScene();
    Cell* c = new Cell();
    scene->addItem(c);
    i++;

    Stitch* s = StitchLibrary::inst()->findStitch(name);
    c->setStitch(s);

    QPointF origin = QPointF(c->boundingRect().width()/2, c->boundingRect().height());
    c->setTransformOriginPoint(origin);
    c->setColor(QColor(color));
    c->setRotation(angle);
    c->setScale(scaleX, scaleY);


    QString rasterImage = "TestCell-AllProperties-" + QString::number(i) + "-" + name + ".png";

    saveScene(scene, QSizeF(width, height), rasterImage);

    QString hexHash = hashFile(rasterImage);

    QCOMPARE(hexHash, rasterHash);

    scene->removeItem(c);
    delete c;
    c = 0;
}

void TestCell::setAllProperties_data()
{

    QTest::addColumn<QString>("name");
    QTest::addColumn<qreal>("width");
    QTest::addColumn<qreal>("height");
    QTest::addColumn<qreal>("angle");
    QTest::addColumn<qreal>("scaleX");
    QTest::addColumn<qreal>("scaleY");
    QTest::addColumn<QString>("color");
    QTest::addColumn<QString>("rasterHash");

    QTest::newRow("ch")    << "ch" << 32.0 << 16.0 << 45.0 << 1.5 << 2.0 << "#0000FF" << "2d56f4703b3f29f1163bdd6197597e3018a004e6";
    QTest::newRow("hdc")   << "hdc" << 32.0 << 64.0 << 45.0 << 1.5 << 2.0 << "#FF0000" << "2bffe89a7dad96f18eae2ded0dc08f7f207ceb30";
    QTest::newRow("dc")    << "dc" << 32.0 << 80.0 << 45.0 << 1.5 << 2.0 << "#00FF00" << "0c151d2b0aee5157a59e1ac769bb26644365ced9";

}

void TestCell::saveScene(QGraphicsScene* scene, QSizeF size, QString fileName)
{

    scene->setSceneRect(scene->itemsBoundingRect());

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
