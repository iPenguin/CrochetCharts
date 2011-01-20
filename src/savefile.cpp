/*************************************************\
| (c) 2011 Stitch Works Software                  |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "savefile.h"
#include "appinfo.h"

#include "charttab.h"

#include <QDebug>

#include <QFile>

#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>

#include <QXmlStreamWriter>
#include <QXmlStreamReader>

#include <QDataStream>
#include "crochetscene.h"
#include "crochetdatamodel.h"
#include "stitchcollection.h"
#include "stitchset.h"

SaveFile::SaveFile(QTabWidget* tabWidget)
    : isSaved(false), fileName(""), mFileVersion(SaveFile::Version_1_0), mTabWidget(tabWidget)
{
}

SaveFile::~SaveFile()
{
}

SaveFile::FileError SaveFile::save()
{
/*
//TODO: add stitches and make this a QDataStream.
    QFile file(fileName);
    file.open(QIODevice::WriteOnly);
    QDataStream out(&file);
    
    // Write a header with a "magic number" and a version
    out << AppInfo::magicNumber;
    out << (qint32)mFileVersion;
    
    out.setVersion(QDataStream::Qt_4_7);
*/

    QString *data = new QString();
    QXmlStreamWriter stream(data);
    stream.setAutoFormatting(true);
    stream.writeStartDocument();

    stream.writeStartElement("pattern"); //start pattern
    //TODO: dont need to set the version when saving into a binary file.
    stream.writeAttribute("version", QString::number(SaveFile::Version_1_0));
    saveChart(&stream);
    stream.writeEndElement();
    
    stream.writeEndDocument();

    QFile file(fileName);

    if(!file.open(QIODevice::WriteOnly)) {
        //TODO: some nice dialog to warn the user.
        qWarning() << "Couldn't open file for writing..." << fileName;
        return SaveFile::Err_OpeningFile;
    }

    file.write(data->toLatin1());
    file.close();
    
    return SaveFile::No_Error;
}

SaveFile::FileError SaveFile::load()
{
/*
//TODO: add stitches and make this a QDataStream.*
    QFile file(fileName);
    file.open(QIODevice::ReadOnly);
    QDataStream in(&file);
    
    // Read and check the header
    quint32 magic;
    in >> magic;
    if (magic != AppInfo::magicNumber)
        return SaveFile::Err_WrongFileType;

    qint32 version;
    in >> version;
    
    if (version <= SaveFile::Version_1_0)
        in.setVersion(QDataStream::Qt_4_7);

    if (version > mFileVersion)
        return SaveFile::Err_UnknownFileVersion;
    
    // Read the data
    //in >> lots_of_interesting_data;
    //if (version >= 120)
    //    in >> data_new_in_XXX_version_1_2;
    //in >> other_interesting_data;
*/
    QFile file(fileName);

    if(!file.open(QIODevice::ReadOnly)) {
        //TODO: some nice dialog to warn the user.
        qWarning() << "Couldn't open file for reading..." << fileName;
        return SaveFile::Err_OpeningFile;
    }
    
    QDomDocument doc("pattern");
    
    if (!doc.setContent(&file)) {
        file.close();
        return SaveFile::Err_GettingFileContents;
    }
    file.close();
    
    QDomElement docElem = doc.documentElement();
    
    QDomNode n = docElem.firstChild();
    while(!n.isNull()) {
        QDomElement e = n.toElement(); // try to convert the node to an element.
        if(!e.isNull()) {
            if(e.tagName() == "chart") {
                loadChart(&e);
            } else if (e.tagName() == "stitch") { // custom stitches.
                continue;
            }
        }
        n = n.nextSibling();
    }
    
    return SaveFile::No_Error;
}

void SaveFile::loadChart(QDomElement *element)
{
    ChartTab* tab = new ChartTab();
    QDomNode n = element->firstChild();
    while(!n.isNull()) {
        QDomElement e = n.toElement();
        if(!e.isNull()) {
            if(e.tagName() == "name") {
                tab->setName(e.text());
            } else if(e.tagName() == "cell") {
                QDomNode nProp = e.firstChild();
                while(!nProp.isNull()) {
                    QDomElement eProp = nProp.toElement();
                    CrochetCell* c = new CrochetCell();
                    if(!eProp.isNull()) {
                        if(eProp.tagName() == "stitch") {
                            Stitch* s = StitchCollection::inst()->masterStitchSet()->findStitch(eProp.text());
                            c->setStitch(s);
                        } else if(eProp.tagName() == "x") {
                            c->pos().setX((qreal) eProp.text().toDouble());
                        } else if(eProp.tagName() == "y") {
                            c->pos().setY((qreal) eProp.text().toDouble());
                        } else if(eProp.tagName() == "rotation") {
                            c->setRotation(eProp.text().toDouble());
                        } else if(eProp.tagName() == "angle") {
                            c->setAngle(eProp.text().toDouble());
                        } else {
                            qWarning() << "Cannot load stitch" << eProp.tagName() << eProp.text();
                        }
                    }
                    qDebug() << "cell" << c->stitch()->name();
                     tab->scene()->addCell(c->pos().x(), c->pos().y(), c);
                    
                    nProp = nProp.nextSibling();
                }
            } else {
                qWarning() << "Cannot load unknown stitch property:" << e.tagName() << e.text();
            }
        }
        n = n.nextSibling();
    }
}

bool SaveFile::saveCustomStitches(QDataStream* stream)
{
    Q_UNUSED(stream);
    return true;
}

bool SaveFile::loadCustomStitches(QDataStream* stream)
{
    Q_UNUSED(stream);
    return true;
}

bool SaveFile::saveChart(QXmlStreamWriter* stream)
{
    int tabCount = mTabWidget->count();
        
    for(int i = 0; i < tabCount; ++i) {
        stream->writeStartElement("chart"); //start chart
        ChartTab* tab = qobject_cast<ChartTab*>(mTabWidget->widget(i));
        if(!tab)
            continue;
        stream->writeTextElement("name", tab->name());
        CrochetDataModel *model = tab->scene()->model();

        int rows = model->rowCount();
        for(int row = 0; row < rows; ++row) {
            int cols = model->columnCount(row);
            for(int col = 0; col < cols; ++col) {
                CrochetCell* c = qgraphicsitem_cast<CrochetCell*>(model->cell(row, col));
                if(!c)
                    continue;
                stream->writeStartElement("cell"); //start cell
                    stream->writeTextElement("stitch", c->stitch()->name());
                    //TODO: make sure the numbers are translating correctly to xml and back.
                    stream->writeTextElement("x", QString::number(c->pos().x()));
                    stream->writeTextElement("y", QString::number(c->pos().y()));
                    stream->writeTextElement("rotation", QString::number(c->rotation()));
                    stream->writeTextElement("angle", QString::number(c->angle()));
                stream->writeEndElement(); //end cell
            }
        }

        stream->writeEndElement(); // end chart
    }

    return true;
}

bool SaveFile::loadChart(QXmlStreamReader* stream)
{
    ChartTab* tab = new ChartTab(mTabWidget);
    while(stream->readNextStartElement()) {

        if(stream->name() == "name") {
            tab->setName(stream->text().toString());
        } else if (stream->name() == "cell") {
            CrochetCell* c = new CrochetCell();

            qDebug() << "cell" << c->stitch()->name();
        } else {
            qWarning() << "Skipped element in the chart file:" << stream->name() << stream->text();
            stream->skipCurrentElement();
        }
        qDebug() << "outer loop" << stream->name() << stream->text();
        stream->skipCurrentElement();
    }
    
    //FIXME: load the tab name in the place of the "Chart".
    mTabWidget->addTab(tab, "Chart");
    return true;
}
