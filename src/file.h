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
#ifndef FILE_H
#define FILE_H

#include "filefactory.h"
#include "mainwindow.h"
#include "stitchset.h"
#include <QTabWidget>

class File
{
public:
    File(MainWindow *mw, FileFactory *parent);

    virtual FileFactory::FileError load(QDataStream *stream) = 0;
    virtual FileFactory::FileError save(QDataStream *stream) = 0;

protected:
    MainWindow *mMainWindow;
    FileFactory *mParent;
    QTabWidget *mTabWidget;
    StitchSet *mInternalStitchSet;
};

#endif // FILE_H
