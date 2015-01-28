/****************************************************************************\
 Copyright (c) 2010-2014 Stitch Works Software
 Brian C. Milco <bcmilco@gmail.com>

 This file is part of Crochet Charts.

 Crochet Charts is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Crochet Charts is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Crochet Charts. If not, see <http://www.gnu.org/licenses/>.

 \****************************************************************************/
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
