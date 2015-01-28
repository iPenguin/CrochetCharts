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
#ifndef DEBUG_H
#define DEBUG_H

#include <QDebug>

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

QString colorizeFunc(QString name);

#define DEBUG(message) \
( \
    (qDebug() << colorizeFunc(Q_FUNC_INFO).toStdString().c_str() << ":" << QString(message).toStdString().c_str()), \
    (void)0 \
)

#define WARN(message) \
( \
    (qWarning() << colorizeFunc(Q_FUNC_INFO).toStdString().c_str() << ":" << QString(message).toStdString().c_str()), \
    (void)0 \
)

#define CRITICAL(message) \
( \
    (qCritical() << colorizeFunc(Q_FUNC_INFO).toStdString().c_str() << ":" << QString(message).toStdString().c_str()),\
    (void)0 \
)

#define FATAL(message) \
( \
    (qFatal("%s : %s", colorizeFunc(Q_FUNC_INFO).toStdString().c_str(), QString(message).toStdString().c_str())), \
    (void)0 \
)

#endif // DEBUG_H
