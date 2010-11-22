#ifndef TESTCROCHETDATAMODEL_H
#define TESTCROCHETDATAMODEL_H

#include <QtTest/QTest>
#include <QDebug>
#include <QObject>

#include "../crochetdatamodel.h"

class TestCrochetDataModel : public QObject
{
    Q_OBJECT
private slots:
     void initTestCase();
     void testDataModelSetup();
     void testDataModelAdditions();
     void testDataModelInsertions();
     void testDataModelRemovals();
     void cleanupTestCase();

private:
     CrochetDataModel *model;
};

#endif // TESTCROCHETDATAMODEL_H
