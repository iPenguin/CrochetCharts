#include "debug.h"

#include <QStringList>
#include <QString>

QString debugFunctionName(QString name)
{

    QString output;
    QStringList function = name.split("::");
    QStringList className = function.first().split(" ");

    output.append("\033[036m");
    output.append(className.first());
    output.append("\033[0m \033[32m");
    output.append(className.last());
    output.append("\033[0m::\033[34m");
    output.append(function.last());
    output.append("\033[0m");
    return output;

}