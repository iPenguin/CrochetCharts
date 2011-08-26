/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef TESTSTITCH_H
#define TESTSTITCH_H

#include <QtTest/QTest>
#include <QDebug>
#include <QObject>

#include "../src/stitch.h"

class TestStitch : public QObject
{
    Q_OBJECT
private slots:
     void initTestCase();
     void stitchSetup();
     void stitchRender();
     void stitchRender_data();
     void cleanupTestCase();

private:
     Stitch* mS;
};

#endif // TESTSTITCH_H
