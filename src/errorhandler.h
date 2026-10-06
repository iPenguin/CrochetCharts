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
#ifndef ERRORHANDLER_H
#define ERRORHANDLER_H

void errorHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    const char *file = context.file ? context.file : "";
    const char *function = context.function ? context.function : "";

    switch (type) {
        case QtDebugMsg:
            fprintf(stderr, "%s:%s: %s\n", file, function, localMsg.constData());
            break;
        case QtWarningMsg:
            fprintf(stderr, "\033[1;33mWarning\033[0m: %s:%s: %s\n", file, function, localMsg.constData());
            break;
        case QtCriticalMsg:
            fprintf(stderr, "\033[31mCritical\033[0m: %s:%s: %s\n", file, function, localMsg.constData());
            break;
        case QtFatalMsg:
            fprintf(stderr, "\033[31mFatal\033[0m: %s:%s: %s\n", file, function, localMsg.constData());
            abort();
    }
}

#endif // ERRORHANDLER_H
