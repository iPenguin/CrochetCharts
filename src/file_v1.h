/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef FINE_V1_H
#define FINE_V1_H

#include "file.h"

#include <QXmlStreamReader>
#include <QXmlStreamWriter>

class QDataStream;
class CrochetTab;
class Scene;

class File_v1 : public File
{
public:
    File_v1(MainWindow *mw, FileFactory *parent);

    FileFactory::FileError load(QDataStream *stream);
    FileFactory::FileError save(QDataStream *stream);

protected:
    void cleanUp();

private:
    void loadColors(QXmlStreamReader *stream);
    void loadChart(QXmlStreamReader *stream);

    void loadCell(CrochetTab *tab, QXmlStreamReader *stream);
    void loadGrid(QXmlStreamReader *stream, Scene *scene);
    void loadIndicator(CrochetTab *tab, QXmlStreamReader *stream);

    void saveCustomStitches(QXmlStreamWriter* stream);
    void saveColors(QXmlStreamWriter* stream);
    bool saveCharts(QXmlStreamWriter* stream);

};
#endif // FINE_V1_H
