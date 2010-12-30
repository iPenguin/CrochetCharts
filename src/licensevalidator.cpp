#include "licensevalidator.h"

#include "license.h"
#include <QDebug>

LicenseValidator::LicenseValidator(QObject *parent)
    : QRegExpValidator(License::licenseRegEx(), parent)
{
}

QValidator::State LicenseValidator::validate(QString &input, int &pos) const
{
    //remove whitespaces before we process the input.
    input = input.simplified();

    QValidator::State state;
    state = QRegExpValidator::validate(input, pos);

    if(state == QValidator::Acceptable) {
        if(!License::isValidSerialNumber(input)) {
            //we make the state intermediate because if we made it invalid it wouldn't
            //display the last digit in the line edit.
            state = QValidator::Intermediate;
        }
    }

    return state;
}
