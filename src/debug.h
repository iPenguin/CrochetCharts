#ifndef DEBUG_H
#define DEBUG_H

#include <QDebug>
#include <QString>

/*
    black="\033[30m";
    dark_gray="\033[01;30m";
    red="\033[31m";
    bright_red="\033[1;31m";
    green="\033[32m";
    bright_green="\033[1;32m";
    yellow="\033[33m";
    bright_yellow="\033[1;33m";
    blue="\033[34m";
    bright_blue="\033[1;34m";
    violet="\033[35m";
    bright_violet="\033[1;35m";
    cyan="\033[036m";
    bright_cyan="\033[1;36m";
    white="\033[37m";
    light_gray="\033[00;37m";
    end_color="\033[0m";

 */

QString debugFunctionName(QString name);

#define DEBUG(message) \
( \
    (qDebug() << debugFunctionName(Q_FUNC_INFO).toStdString().c_str() << ":\033[01;30m" << QString(message).toStdString().c_str() << "\033[0m"), \
    (void)0 \
)

#define WARN(message) \
( \
    (qWarning() << debugFunctionName(Q_FUNC_INFO).toStdString().c_str() << ":\033[1;33m" << QString(message).toStdString().c_str() << "\033[0m"), \
    (void)0 \
)

#define CRITICAL(message) \
( \
    (qCritical() << debugFunctionName(Q_FUNC_INFO).toStdString().c_str() << ":\033[31m" << QString(message).toStdString().c_str() << "\033[0m"), \
    (void)0 \
)

#define FATAL(message) \
( \
    (qFatal("%s : \033[31m%s\033[0m", debugFunctionName(Q_FUNC_INFO).toStdString().c_str(), QString(message).toStdString().c_str())), \
    (void)0 \
)

#endif // DEBUG_H
