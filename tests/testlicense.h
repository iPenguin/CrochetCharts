/*************************************************\
| (c) 2010-2011 Stitch Works Software             |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef TESTLICENSE_H
#define TESTLICENSE_H

#include <QtTest/QTest>
#include <QDebug>
#include <QObject>

class TestLicense : public QObject
{
    Q_OBJECT
private slots:
     void initTestCase();

     void checkSerialNumbers();
     void checkSerialNumbers_data();

     void checkLicenseNumbers();
     void checkLicenseNumbers_data();

     void cleanupTestCase();

private:
};

#endif // TESTLICENSE_H
