/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef TESTSTITCHLIBRARY_H
#define TESTSTITCHLIBRARY_H

#include <QtTest/QTest>
#include <QObject>
#include "../src/stitchlibrary.h"

class TestStitchLibrary : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void findStitch();
    void findStitch_data();
    void cleanupTestCase();

private:
};

#endif //TESTSTITCHLIBRARY_H
