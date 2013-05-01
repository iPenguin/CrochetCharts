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
