#include "testlicense.h"

#include "../src/license.h"

void TestLicense::initTestCase()
{
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

    QTest::newRow("one")                  << long(1)      << "0000-001-0533";
    QTest::newRow("ten")                  << long(10)     << "0000-010-3243";
    QTest::newRow("one hundred")          << long(100)    << "0000-100-8535";
    QTest::newRow("one thousand")         << long(1000)   << "0001-000-3802";
    QTest::newRow("ten thousand")         << long(10000)  << "0010-000-1110";
    QTest::newRow("one hundred thousand") << long(100000) << "0100-000-1912";

}

void TestLicense::cleanupTestCase()
{
}
