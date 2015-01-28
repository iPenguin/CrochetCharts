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
#include "licensevalidator.h"

#include "license.h"
#include <QDebug>

LicenseValidator::LicenseValidator(QObject* parent)
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
