#ifndef TESTCROCHETDATAMODEL_H
#define TESTCROCHETDATAMODEL_H

#include <QtTest/QTest>
#include <QDebug>
#include <QObject>

#include "../src/stitchset.h"

class TestStitchSet : public QObject
{
    Q_OBJECT
private slots:
     void initTestCase();
     void setupStitchSet();
     void findStitch();
     void 
     void cleanupTestCase();

private:
     Stitch *s;
};

#endif // TESTCROCHETDATAMODEL_H
