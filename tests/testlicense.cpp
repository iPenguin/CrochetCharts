/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
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

    QTest::newRow("one")            << "0000-001-0203" << "bcmilco@gmail.com" << "8B14-D834-3FC1-L1" << true;
    QTest::newRow("ten")            << "0000-010-9313" << "bcmilco@gmail.com" << "360E-ED3D-9BA0-L1" << true;
    QTest::newRow("one hundred")    << "0000-100-5356" << "bcmilco@gmail.com" << "58BC-AF41-B16A-L1" << true;
    QTest::newRow("one thousand")   << "0001-000-9650" << "bcmilco@gmail.com" << "ECC0-A35B-5C35-L1" << true;
    QTest::newRow("ten thousand")   << "0010-000-7097" << "bcmilco@gmail.com" << "90A8-33FE-FF99-L1" << true;
    QTest::newRow("one-h thousand") << "0100-000-5861" << "bcmilco@gmail.com" << "D049-B534-AD04-L1" << true;
    QTest::newRow("one million")    << "1000-000-5179" << "bcmilco@gmail.com" << "051D-F96B-26EF-L1" << true;
    QTest::newRow("9,999,999")      << "9999-999-7752" << "bcmilco@gmail.com" << "00B8-7C1E-0676-L1" << true;

    QTest::newRow("trial - gmail")   << "" << "bcmilco@gmail.com" << "7CC0-1DB8-641B-T1" << true;
    QTest::newRow("trial - hotmail") << "" << "bcmilco@hotmail.com" << "4827-F9BD-8DF2-T1" << true;
    QTest::newRow("trial - sws")     << "" << "brian@stitchworkssoftware.com" << "1E3B-18CE-2ED1-T1" << true;

    QTest::newRow("L1 invalid - license")  << "0000-001-0203" << "bcmilco@gmail.com" << "C539-8507-1102-L1" << false;
    QTest::newRow("L1 invalid - email")    << "0000-001-0203" << "bcmilco@gmail" << "C539-8507-7080-L1" << false;
    QTest::newRow("L1 invalid - sn")       << "0000-001-0533" << "bcmilco@gmail.com" << "C539-8507-7080-L1" << false;

    QTest::newRow("T1 invalid - license")  << "" << "bcmilco@gmail.com" << "C539-8105-7080-L1" << false;
    QTest::newRow("T1 invalid - email")    << "" << "bcmilco@gmcom" << "C539-8507-7080-L1" << false;
    QTest::newRow("T1 invalid - sn")       << "0000-001-0533" << "bcmilco@gmail.com" << "C539-8507-7080-L1" << false;
}

void TestLicense::cleanupTestCase()
{
}
