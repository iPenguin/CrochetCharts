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
