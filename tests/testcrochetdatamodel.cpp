/*************************************************\
| (c) 2010-2011 Stitch Works Software             |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "testcrochetdatamodel.h"

void TestCrochetDataModel::initTestCase()
{
    model = new CrochetDataModel(this);
}

void TestCrochetDataModel::testDataModelSetup()
{
    model->setRowCount(10);
    model->setColumnCount(0, 10);

    QVERIFY(model->rowCount() == 10);
    QVERIFY(model->columnCount(0) == 10);
}

void TestCrochetDataModel::testDataModelAppendRows()
{
    model->appendRow();

    QVERIFY(model->rowCount() == 11);
}

void TestCrochetDataModel::testDataModelInsertRows()
{

}

void TestCrochetDataModel::testDataModelRemoveRows()
{

}

void TestCrochetDataModel::testDataModelAppendColumns()
{

}

void TestCrochetDataModel::testDataModelInsertColumns()
{

}

void TestCrochetDataModel::testDataModelRemoveColumns()
{

}

void TestCrochetDataModel::testDataModelSet()
{

}

void TestCrochetDataModel::testDataModelAt()
{

}

void TestCrochetDataModel::cleanupTestCase()
{
}
