/*************************************************\
| (c) 2010-2011 Stitch Works Software             |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef LICENSE_H
#define LICENSE_H

#include <QCryptographicHash>
#include <math.h>
#include <QList>
#include <QString>
#include <QRegExp>
#include <QMap>

namespace License {

    QRegExp licenseRegEx();
    QMap<QString, QString> licenseGenValues(bool validSn = false, bool validEmail = false);
    QString serialNumber(long sequenceNumber);
    bool isValidSerialNumber(QString sn);

    QString generateLicense(QString serialNumber, QString email);
    bool isValidLicense(QString license, QString serialNumber, QString email);

    bool isValidEmail(QString email);
}
#endif // LICENSE_H
