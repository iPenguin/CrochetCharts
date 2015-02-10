/****************************************************************************\
 Copyright (c) 2010-2014 Stitch Works Software
 Brian C. Milco <bcmilco@gmail.com>

 This file is part of Crochet Charts.

 Crochet Charts is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Crochet Charts is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Crochet Charts. If not, see <http://www.gnu.org/licenses/>.

 \****************************************************************************/
#include "testcell.h"
#include "../src/stitchlibrary.h"

#include <QPainter>
#include <QFile>
#include <QCryptographicHash>
#include <QSvgGenerator>

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

    saveSceneSvg(scene, QSizeF(width, height), rasterImage);

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

    QTest::newRow("ch")    << "ch" << 32.0 << 16.0 << 45.0 << "8bc84f266a6b29c060e99e80cc60f5a4dd1c0db9";
    QTest::newRow("hdc")   << "hdc" << 32.0 << 64.0 << 45.0 << "4a7eac0d444a5d98f80ed44cd03be3c85ea8f2c1";
    QTest::newRow("dc")    << "dc" << 32.0 << 80.0 << 45.0 << "26fcac78640d8b5dfe7a350dcfa51f00948e80bb";

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
    c->setBgColor(QColor(color));

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
    c->setBgColor(QColor(color));
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

    QTest::newRow("ch")    << "ch" << 32.0 << 16.0 << 45.0 << 1.5 << 2.0 << "#0000FF"
                                    << "2d56f4703b3f29f1163bdd6197597e3018a004e6";
    QTest::newRow("hdc")   << "hdc" << 32.0 << 64.0 << 45.0 << 1.5 << 2.0 << "#FF0000"
                                    << "2bffe89a7dad96f18eae2ded0dc08f7f207ceb30";
    QTest::newRow("dc")    << "dc" << 32.0 << 80.0 << 45.0 << 1.5 << 2.0 << "#00FF00"
                                    << "0c151d2b0aee5157a59e1ac769bb26644365ced9";

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

void TestCell::saveSceneSvg(QGraphicsScene* scene, QSizeF size, QString fileName)
{
    scene->setSceneRect(scene->itemsBoundingRect());

    QPainter p;
    QSvgGenerator gen;
    gen.setFileName(fileName);
    gen.setSize(size.toSize());
    gen.setViewBox(QRectF(0,0,size.width(),size.height()));

    p.begin(&gen);
    scene->render(&p);
    p.end();

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
