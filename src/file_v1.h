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
