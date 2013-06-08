/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "file.h"

File::File(MainWindow *mw, FileFactory *parent) :
    mMainWindow(mw),
    mParent(parent),
    mInternalStitchSet(0)
{
    mTabWidget = mMainWindow->tabWidget();

}
