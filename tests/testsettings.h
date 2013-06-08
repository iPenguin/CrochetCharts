/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef TESTSETTINGS_H
#define TESTSETTINGS_H

#include <QtTest/QTest>
#include <QDebug>
#include <QObject>

#include "../src/settings.h"
#include "../src/appinfo.h"

class TestSettings : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void setSettings();
    void setSettings_data();
    void cleanupTestCase();

private:

};
#endif //TESTSETTINGS_H
