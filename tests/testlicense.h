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
     void checkLicense();
     void checkLicense_data();
     void cleanupTestCase();

private:
};

#endif // TESTLICENSE_H
