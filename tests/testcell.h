/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef TESTCELL_H
#define TESTCELL_H

#include <QtTest/QTest>
#include <QDebug>
#include <QObject>

#include <QGraphicsView>
#include <QGraphicsScene>

#include "../src/cell.h"

class TestCell : public QObject
{
    Q_OBJECT
private slots:
     void initTestCase();

     void setCellValues();
     void setCellValues_data();

     void setRotation();
     void setRotation_data();

     void setScale();
     void setScale_data();

     void setBgColor();
     void setBgColor_data();

     void setAllProperties();
     void setAllProperties_data();

     void cleanupTestCase();

private:
     int i;
     //Cell* mCell;

     //QGraphicsScene* scene;

     void saveScene(QGraphicsScene *scene, QSizeF size, QString fileName);
     QString hashFile(QString fileName);
};

#endif // TESTCELL_H
