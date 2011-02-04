/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef TESTCELL_H
#define TESTCELL_H

#include <QtTest/QTest>
#include <QDebug>
#include <QObject>

#include "../src/crochetcell.h"

class TestCell : public QObject
{
    Q_OBJECT
private slots:
     void initTestCase();
     void setupCell();
     void setCellValues();
     void cleanupTestCase();

private:
     Cell *mCell;
};

#endif // TESTCELL_H
