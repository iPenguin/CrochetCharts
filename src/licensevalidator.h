/*************************************************\
| (c) 2010-2011 Stitch Works Software             |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef LICENSEVALIDATOR_H
#define LICENSEVALIDATOR_H

#include <QRegExpValidator>

class LicenseValidator : public QRegExpValidator
{
    Q_OBJECT
public:
    LicenseValidator(QObject *parent);

    QValidator::State validate(QString &input, int &pos) const;
};

#endif //LICENSEVALIDATOR_H
