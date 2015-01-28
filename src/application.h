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
#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include <QStringList>

#include "mainwindow.h"

class Application : public QApplication
{
    Q_OBJECT
public:
    Application(int &argc, char **argv);

    static void resendFileOpenEvents(QObject *receiver);

    void setMainWindow(MainWindow *window);

protected:
    bool event(QEvent *event);

private:
    void loadFile(const QString &fileName);
    static QStringList mFileOpenEventList;
    MainWindow *mW;
};

#endif //APPLICATION_H
