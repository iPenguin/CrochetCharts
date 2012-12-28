#include "debug.h"
#include <QStringList>

QString debugFunctionName(QString name)
{
    QString output = "";
    QStringList functionAndParams = name.split("(");
    
    QStringList classParts = functionAndParams.first().split("::");
    QStringList nameAndReturnType = classParts.first().split(" ");
    
    QString returnType = ""; //ctor, dtor don't have return types
    QString virtualKeyWord = "";
    if(nameAndReturnType.count() > 2) {
        virtualKeyWord = nameAndReturnType.first() + " ";
        returnType = nameAndReturnType.at(2) + " ";
    } else if(nameAndReturnType.count() > 1) {
        returnType = nameAndReturnType.first() + " ";
    }
    
    QString className = nameAndReturnType.last();
    QString functionName = classParts.last().split("(").first();
    
    QStringList paramsAndConst = functionAndParams.last().split(")");
    QStringList params = paramsAndConst.first().split(",");
    QString constString = paramsAndConst.last();
    
    if(!virtualKeyWord.isEmpty()) {
        output.append("\033[1;33m");
        output.append(virtualKeyWord);
    }
    output.append("\033[036m");
    output.append(returnType);
    output.append("\033[0m\033[32m");
    output.append(className);
    output.append("\033[0m::");
    output.append("\033[34m");
    output.append(functionName);
    
    output.append("\033[0m(");
    
    if(params.count() > 0) {
        QStringList::const_iterator param;
        for (param = params.begin(); param != params.constEnd(); ++param) {
            if(param != params.begin()) {
                output.append("\033[0m,");
            }
            output.append("\033[036m");
            output.append((*param));
        }
    }
    output.append("\033[0m)");
    
    if(!constString.isEmpty()) {
        output.append("\033[1;33m");
        output.append(constString);
        
    }
    output.append("\033[0m");
    
    return output;
}