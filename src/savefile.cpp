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
    
    QXmlStreamReader stream;
    stream.setDevice(&file);

    stream.readNextStartElement();

    QString version = stream.attributes().value("version").toString();
    if (stream.name() == "pattern" && version.toInt() == SaveFile::Version_1_0) {

        while(stream.readNextStartElement()) {
            qDebug() << stream.name();
            if(stream.name() == "chart")
                loadChart(&stream);
            else if(stream.name() == "stitch") {//custom stitch
                continue;
            } else {
                qWarning() << "Could not parse the file:" << stream.name() << stream.text();
                
            }
        }
    } else {
        stream.raiseError(QObject::tr("The file is not an XBEL version 1.0 file."));
    }
    
    file.close();

    return SaveFile::No_Error;
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

            while(stream->readNextStartElement()) {
                if(stream->name() == "stitch") {
                    StitchSet* set = StitchCollection::inst()->masterStitchSet();
                    //TODO: what to do if I cannot find the stitch?
                    c->setStitch(set->findStitch(stream->text().toString()));
                } else if(stream->name() == "x") {
                    bool ok = false;
                    qreal temp = stream->text().toString().toDouble(&ok);
                    if(ok)
                        c->pos().setX(temp);
                    else
                        qWarning() << "Couldn't read stitch's x position";
                } else if(stream->name() == "y") {
                    bool ok = false;
                    qreal temp = stream->text().toString().toDouble(&ok);
                    if(ok)
                        c->pos().setY(temp);
                    else
                        qWarning() << "Couldn't read stitch's y position";
                } else if(stream->name() == "rotation")  {
                    bool ok = false;
                    double temp = stream->text().toString().toDouble(&ok);
                    if(ok)
                        c->setRotation(temp);
                    else
                        qWarning() << "Couldn't read stitch's rotation";
                } else if(stream->name() == "angle")  {
                    bool ok = false;
                    double temp = stream->text().toString().toDouble(&ok);
                    if(ok)
                        c->setAngle(temp);
                    else
                        qWarning() << "Couldn't read stitch's angle";
                } else {
                    qWarning() << "Skipped cell element in the chart file:" << stream->name() << stream->text();
                    stream->skipCurrentElement();
                }
                qDebug() << "inner loop" << stream->readElementText();
                stream->skipCurrentElement();
            }
        } else {
            qWarning() << "Skipped element in the chart file:" << stream->name() << stream->text();
            stream->skipCurrentElement();
        }
        qDebug() << "outer loop" << stream->readElementText();
        stream->skipCurrentElement();
    }
    
    //FIXME: load the tab name in the place of the "Chart".
    mTabWidget->addTab(tab, "Chart");
    return true;
}
