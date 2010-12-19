#include "teststitch.h"

#include "../src/license.h"

void TestLicense::initTestCase()
{
    qDebug() << "Begin all tests";
}

void TestLicense::checkLicense()
{

    QFETCH(long, sequence);
    QFETCH(QString, serial_number);

    QCOMPARE(License::serialNumber(sequence), serial_number);
  
}

void TestLicense::checkLicense_data()
{
    QTest::addColumn<long>("sequence");
    QTest::addColumn<QString>("serial_number");

    QTest::newRow("1")      << 1      << "0000-001-3355";
    QTest::newRow("10")     << 10     << "0000-010-7746";
    QTest::newRow("100")    << 100    << "0000-100-4162";
    QTest::newRow("1000")   << 1000   << "0001-000-2595";
    QTest::newRow("10000")  << 10000  << "0010-000-6783";
    QTest::newRow("100000") << 100000 << "0100-000-9983";

}

void TestLicense::cleanupTestCase()
{
    qDebug() << "End all tests";
}


//main()
QTEST_MAIN(TestLicense)
