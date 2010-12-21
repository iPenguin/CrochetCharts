#include "testcrochetdatamodel.h"

void TestCrochetDataModel::initTestCase()
{
    qDebug() << "Begin all tests";
    model = new CrochetDataModel(this);
}

void TestCrochetDataModel::testDataModelSetup()
{

    model->setRows(10);
    model->setInitialColumns(10);

    QVERIFY(model->rowCount() == 10);
    QVERIFY(model->columnCount(0) == 10);

    model->addColumn(1, 10);

    QVERIFY(model->columnCount(1) == 11);

    model->addColumn(2, 10);
    model->addColumn(2);

    //QVERIFY(model->cell(3, 6) == "1");

}

void TestCrochetDataModel::testDataModelAdditions()
{
    //model->setCell(0, 0, );

    //QVERIFY(model->cell(0, 0) == );

}

void TestCrochetDataModel::testDataModelInsertions()
{


}

void TestCrochetDataModel::testDataModelRemovals()
{

    model->removeColumn(0, 2);
/*
    QVERIFY(model->cell(0, 0) == "a");
    QVERIFY(model->cell(0, 1) == "x");
    QVERIFY(model->cell(0, 2) == "z");
*/
}

void TestCrochetDataModel::cleanupTestCase()
{
    qDebug() << "End all tests";
}
