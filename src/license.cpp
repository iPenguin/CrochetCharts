/*************************************************\
| (c) 2010-2011 Stitch Works Software             |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "license.h"

#include <QDebug>

QRegExp License::licenseRegEx()
{
    return QRegExp("[0-9]{4}-[0-9]{3}-[0-9]{4}");
}

QMap<QString, QString> License::licenseGenValues(bool validSn, bool validEmail)
{
    QMap<QString, QString> data;
    if(!validSn && !validEmail) {                  //serial number generator values
        data["seed"] = ";;Stitch Works Software - Serial Number;;";
    } else if(validSn) {                           //full license values
        data["license_type"] = "L1";
        data["seed"] = ";;Stitch Works Software - Full License;;";
    } else if(!validSn && validEmail) {            //trial license values
        data["license_type"] = "T1";
        data["seed"] = ";;Stitch Works Software - Trial License;;";
    }

    return data;
}

//this function takes a long int and returns a QString serial number in the form
//4444-333-1234
QString License::serialNumber(long sequenceNumber)
{
    QByteArray seed = License::licenseGenValues(false, false)["seed"].toLatin1();
    QString number = QString::number(sequenceNumber);
    QByteArray data = seed + number.toUtf8();

    QByteArray hash = QCryptographicHash::hash(data, QCryptographicHash::Sha1);
    
    QString hashString = hash.toHex();

    QList<int> hexVals;
    bool ok;
    
    //get the first 2 and the last 2 hex digits of the hash.
    QString str = hashString.mid(0,2);
    hexVals.append(str.left(1).toInt(&ok, 16));
    hexVals.append(str.right(1).toInt(&ok, 16));
    
    str = hashString.mid(hashString.length() - 2, 2);
    hexVals.append(str.left(1).toInt(&ok, 16));
    hexVals.append(str.right(1).toInt(&ok, 16));

    QList<int> digits;
    
    //convert each hex char to a base 10 int.
    for(int i = 0; i < hexVals.count(); ++i) {
        int temp = hexVals[i] / 10;
        if(temp < 1)
            digits.append(hexVals[i]);
        else if (temp >= 1)
            digits.append(hexVals[i] - 10);
    }
    
    //create the padded sequence number w/o the hash.
    QString seqNum = QString("%1").arg(QString::number(sequenceNumber),7, QChar('0'));
    
    //break the sequence number into 4444-333 and append the hash (-1234).
    QString sn;
    sn = seqNum.left(4) + "-" + seqNum.right(3) + "-";
    foreach(int digit, digits)
        sn.append(QString::number(digit));
    
    return sn;
}

bool License::isValidSerialNumber(QString sn)
{
    
    if(sn.length() > 13)
        return false;
    if(sn.length() < 13)
        return false;
    
    QString seq = sn.left(8).remove("-");
    long num = seq.toLong();
    
    QString test = serialNumber(num);
    if(sn == test)
        return true;
    return false;
}

QString License::generateLicense(QString serialNumber, QString email)
{
    bool validSn = isValidSerialNumber(serialNumber);
    bool validEmail = isValidEmail(email);

    if(!validSn && !validEmail)
        return "";

    QMap<QString, QString> info = License::licenseGenValues(validSn, validEmail);
    QString seed = info["seed"];
    QString licenseType = info["license_type"];

    QByteArray data = seed.toLatin1() + serialNumber.toLatin1() + email.toLatin1() + licenseType.toLatin1();

    QByteArray hash = QCryptographicHash::hash(data, QCryptographicHash::Sha1);
    QString hashHex = hash.toHex();

    QString beginning = hashHex.mid(0, 4);
    QString middle    = hashHex.mid(18, 4);
    QString end       = hashHex.mid(hashHex.length() - 4, 4);
    QString license = beginning + "-" + middle + "-" + end + "-" + licenseType;

    return license.toUpper();
}

bool License::isValidLicense(QString license, QString serialNumber, QString email)
{
    if(license.isEmpty() || license.isNull())
        return false;

    QString temp = generateLicense(serialNumber, email);

    if(license == temp)
        return true;
    else
        return false;
}

bool License::isValidEmail(QString email)
{
    return true; //TODO: validate the email address.
}
