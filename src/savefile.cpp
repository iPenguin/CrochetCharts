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
SaveFile::FileError SaveFile::load()
{   
    QFile file(fileName);

    if(!file.open(QIODevice::ReadOnly)) {
        //TODO: some nice dialog to warn the user.
        qWarning() << "Couldn't open file for reading..." << fileName;
        return SaveFile::Err_OpeningFile;
    }
    
    QDomDocument doc("pattern");
    
    if (!doc.setContent(&file)) {
        file.close();
        qWarning() << "Couldn't set the contents of the xml file";
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
    QString tabName;

    QDomNode n = element->firstChild();
    while(!n.isNull()) {
        QDomElement e = n.toElement();
        if(!e.isNull()) {
            if(e.tagName() == "name") {
                tabName = e.text();
                tab->setName(tabName);
            } else if(e.tagName() == "cell") {
                loadCell(tab, &e);
            } else {
                qWarning() << "Cannot load unknown stitch property:" << e.tagName() << e.text();
            }
        }
        n = n.nextSibling();
    }
    //FIXME: don't hard code the chart name
    mTabWidget->addTab(tab, tabName);
}

void SaveFile::loadCell(ChartTab* tab, QDomElement *element)
{
    CrochetCell* c = new CrochetCell();
    int row, column;
    qreal x, y, rotation;
    double angle;

    QObject::connect(c, SIGNAL(stitchChanged(QString,QString)), tab->scene(), SIGNAL(stitchChanged(QString,QString)));
    
    QDomNode n = element->firstChild();
    while(!n.isNull()) {
        QDomElement e = n.toElement();
        if(!e.isNull()) {
            if(e.tagName() == "stitch") {
                Stitch* s = StitchCollection::inst()->masterStitchSet()->findStitch(e.text());
                c->setStitch(s);
            } else if(e.tagName() == "row") {
                row = e.text().toInt();
            } else if(e.tagName() == "column") {
                column = e.text().toInt();
            } else if(e.tagName() == "x") {
                x = e.text().toInt();
            } else if(e.tagName() == "y") {
                y = e.text().toInt();
            } else if(e.tagName() == "rotation") {
                rotation = e.text().toDouble();
            } else if(e.tagName() == "angle") {
                angle = e.text().toDouble();
            } else {
                qWarning() << "Cannot load stitch" << e.tagName() << e.text();
            }
        }
        n = n.nextSibling();
    }
    
    tab->scene()->appendCell(row, c);
    c->setPos(x, y);
    c->setRotation(rotation);
    c->setAngle(angle);
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
 
        int rows = tab->scene()->rowCount();

        for(int row = 0; row < rows; ++row) {
            int cols = tab->scene()->columnCount(row);

            for(int col = 0; col < cols; ++col) {
                CrochetCell* c = qgraphicsitem_cast<CrochetCell*>(tab->scene()->cell(row, col));
                if(!c)
                    continue;
                stream->writeStartElement("cell"); //start cell
                    stream->writeTextElement("stitch", c->stitch()->name());
                    stream->writeTextElement("row", QString::number(row));
                    stream->writeTextElement("column", QString::number(col));
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

