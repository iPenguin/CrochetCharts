#ifndef TESTSTITCHSET_H
#define TESTSTITCHSET_H

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
     void cleanupTestCase();

private:

};

#endif // TESTSTITCHSET_H
