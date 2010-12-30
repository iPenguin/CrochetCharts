#include "testlicense.h"

#include "../src/license.h"

void TestLicense::initTestCase()
{
}

void TestLicense::checkSerialNumbers()
{
    QFETCH(long, sequence);
    QFETCH(QString, serial_number);

    QCOMPARE(License::serialNumber(sequence), serial_number); 
}

void TestLicense::checkSerialNumbers_data()
{
    QTest::addColumn<long>("sequence");
    QTest::addColumn<QString>("serial_number");

    QTest::newRow("one")                  << long(1)       << "0000-001-0203";
    QTest::newRow("ten")                  << long(10)      << "0000-010-9313";
    QTest::newRow("one hundred")          << long(100)     << "0000-100-5356";
    QTest::newRow("one thousand")         << long(1000)    << "0001-000-9650";
    QTest::newRow("ten thousand")         << long(10000)   << "0010-000-7097";
    QTest::newRow("one hundred thousand") << long(100000)  << "0100-000-5861";
    QTest::newRow("one million")          << long(1000000) << "1000-000-5179";
    QTest::newRow("9,999,999")            << long(9999999) << "9999-999-7752";

}

void TestLicense::checkLicenseNumbers()
{
    QFETCH(QString, serial_number);
    QFETCH(QString, email);
    QFETCH(QString, license);
    QFETCH(bool, valid_license);

    if(valid_license)
        QCOMPARE(License::generateLicense(serial_number, email), license);

    QCOMPARE(License::isValidLicense(license, serial_number, email), valid_license);
}

void TestLicense::checkLicenseNumbers_data()
{
    QTest::addColumn<QString>("serial_number");
    QTest::addColumn<QString>("email");
    QTest::addColumn<QString>("license");
    QTest::addColumn<bool>("valid_license");

    QTest::newRow("one")            << "0000-001-0203" << "bcmilco@gmail.com" << "C539-8507-7080-L1" << true;
    QTest::newRow("ten")            << "0000-010-9313" << "bcmilco@gmail.com" << "EEAA-4285-888D-L1" << true;
    QTest::newRow("one hundred")    << "0000-100-5356" << "bcmilco@gmail.com" << "568C-5F60-C9B4-L1" << true;
    QTest::newRow("one thousand")   << "0001-000-9650" << "bcmilco@gmail.com" << "AAAD-F075-F9FB-L1" << true;
    QTest::newRow("ten thousand")   << "0010-000-7097" << "bcmilco@gmail.com" << "2957-D7F8-8563-L1" << true;
    QTest::newRow("one-h thousand") << "0100-000-5861" << "bcmilco@gmail.com" << "B18B-F971-0DB9-L1" << true;
    QTest::newRow("one million")    << "1000-000-5179" << "bcmilco@gmail.com" << "54AB-41B9-E2E0-L1" << true;
    QTest::newRow("9,999,999")      << "9999-999-7752" << "bcmilco@gmail.com" << "DDAA-2D64-DC52-L1" << true;

    QTest::newRow("trial - gmail")   << "" << "bcmilco@gmail.com" << "3940-3497-BC8E-T1" << true;
    QTest::newRow("trial - hotmail") << "" << "bcmilco@hotmail.com" << "3940-3497-BC8E-T1" << true;
    QTest::newRow("trial - sws")     << "" << "brian@stitchworkssoftware.com" << "3940-3497-BC8E-T1" << true;

    QTest::newRow("L1 invalid - license");
    QTest::newRow("L1 invalid - email");
    QTest::newRow("L1 invalid - serial number");
    QTest::newRow("T1 invalid");
}

void TestLicense::cleanupTestCase()
{
}
