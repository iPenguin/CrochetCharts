#include "license.h"

#include <QDebug>

QRegExp License::licenseRegEx()
{
    return QRegExp("[0-9]{4}-[0-9]{3}-[0-9]{4}");
}

//this function takes a long int and returns a QString serial number in the form
//4444-333-1234
QString License::serialNumber(long sequenceNumber)
{
    QByteArray seed = "BCM Software";
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

bool License::isValid(QString sn)
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

QString License::license(QString serialNumber, QString email)
{
    if(!isValid(serialNumber))
        return;
    if(!isValidEmail(email))


/*
    if(!valid_sn && !valid_email)
        return '';

    $info = license_gen_values($valid_sn, $valid_email);
    $license_type = $info['license_type'];
    $seed = $info['seed'];

    $data = $seed . $serial_number . $license_type;
    $hash = sha1($data);
    $beginning = substr($hash, 0, 4);
    $middle = substr($hash, 18, 4);
    $end = substr($hash, strlen($hash) - 4, 4);

    $license = $beginning."-".$middle."-".$end."-".$license_type;

    $license = strtoupper($license);

    return $license;
*/
}

bool License::isValidLicense(QString license)
{

}
