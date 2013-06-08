/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
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
