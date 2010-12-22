#ifndef LICENSE_H
#define LICENSE_H

#include <QCryptographicHash>
#include <math.h>
#include <QList>
#include <QString>
#include <QRegExp>

namespace License {

    QRegExp licenseRegEx();
    QString serialNumber(long sequenceNumber);
    bool isValid(QString sn);

}
#endif // LICENSE_H
