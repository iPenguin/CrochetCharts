/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef FINE_V2_H
#define FINE_V2_H

#include "file.h"

#include <QXmlStreamReader>
#include <QXmlStreamWriter>

class QDataStream;
class CrochetTab;
class Scene;

class FileLoad_v2 : public File
{
public:
    FileLoad_v2(FileFactory* parent);

    FileFactory::FileError load(QXmlStreamReader *stream);

protected:

private:
    void loadColors(QXmlStreamReader* stream);
    void loadChart(QXmlStreamReader* stream);

    void loadCell(CrochetTab* tab, QXmlStreamReader* stream);
    void loadGrid(QXmlStreamReader* stream, Scene* scene);
    void loadIndicator(CrochetTab* tab, QXmlStreamReader* stream);

};
#endif // FINE_V2_H
