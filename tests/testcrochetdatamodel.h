#ifndef TESTCROCHETDATAMODEL_H
#define TESTCROCHETDATAMODEL_H

#include <QtTest/QTest>
#include <QDebug>
#include <QObject>

#include "../src/crochetdatamodel.h"

class TestCrochetDataModel : public QObject
{
    Q_OBJECT
private slots:
     void initTestCase();
     void testDataModelSetup();
     void testDataModelAppendRows();
     void testDataModelInsertRows();
     void testDataModelRemoveRows();
     void testDataModelAppendColumns();
     void testDataModelInsertColumns();
     void testDataModelRemoveColumns();
     void testDataModelSet();
     void testDataModelAt();
     void cleanupTestCase();

private:
     CrochetDataModel *model;
};

#endif // TESTCROCHETDATAMODEL_H
