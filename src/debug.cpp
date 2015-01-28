/****************************************************************************\
 Copyright (c) 2011-2014 Stitch Works Software
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
#include "debug.h"

#include <QStringList>
#include <QString>

QString colorizeFunc(QString name)
{
    QString output;
    QStringList classParts = name.split("::");
    QStringList nameAndType = classParts.first().split(" ");

    QString returnType = "";
    if(nameAndType.count() > 1)
        returnType = nameAndType.first() + " ";
    QString className = nameAndType.last();

    QStringList funcAndParamas = classParts.last().split("(");
    funcAndParamas.last().chop(1);
    QString functionName = funcAndParamas.first();
    QStringList params = funcAndParamas.last().split(",");

    output.append("\033[036m");
    output.append(returnType);
    output.append("\033[0m\033[32m");
    output.append(className);
    output.append("\033[0m::");
    output.append("\033[34m");
    output.append(functionName);
    output.append("\033[0m(");

    QStringList::const_iterator param;
    for (param = params.begin(); param != params.constEnd(); ++param) {
        if(param != params.begin()) {
            output.append("\033[0m,");
        }
        output.append("\033[036m");
        output.append((*param));
    }
    output.append("\033[0m)");
    return output;

}
