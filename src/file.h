/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef FILE_H
#define FILE_H

#include "filefactory.h"

class File
{
public:
    File(FileFactory* parent);

    virtual FileFactory::FileError load(QXmlStreamReader *stream) = 0;

protected:
    FileFactory* mParent;
};

#endif // FILE_H
