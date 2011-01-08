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
    QSKIP("Skip this test because it doesn't have anything to do yet.", SkipSingle);
}

void TestCrochetDataModel::testDataModelRemoveRows()
{
    QSKIP("No test here.", SkipSingle);
}

void TestCrochetDataModel::testDataModelAppendColumns()
{
    QSKIP("No test here.", SkipSingle);
}

void TestCrochetDataModel::testDataModelInsertColumns()
{
    QSKIP("No test here.", SkipSingle);
}

void TestCrochetDataModel::testDataModelRemoveColumns()
{
    QSKIP("No test here.", SkipSingle);
}

void TestCrochetDataModel::testDataModelSet()
{
    QSKIP("No test here.", SkipSingle);
}

void TestCrochetDataModel::testDataModelAt()
{
    QSKIP("No test here.", SkipSingle);
}

void TestCrochetDataModel::cleanupTestCase()
{
}
