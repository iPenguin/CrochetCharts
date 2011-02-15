/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
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

#include "mainwindow.h"

SaveFile::SaveFile(QWidget* parent)
    : isSaved(false), fileName(""), mFileVersion(SaveFile::Version_1_0), mParent(parent)
{
    mTabWidget = static_cast<MainWindow*>(mParent)->tabWidget();
}

SaveFile::~SaveFile()
{
}

/*File Save:
 //TODO: add stitches and make this a QDataStream.
 QFile file(fileName);
 file.open(QIODevice::WriteOnly);
 QDataStream out(&file);
 
 // Write a header with a "magic number" and a version
 out << AppInfo::magicNumber;
 out << (qint32)mFileVersion;
 
 out.setVersion(QDataStream::Qt_4_7);
 */
SaveFile::FileError SaveFile::save()
{
    QString *data = new QString();
    QXmlStreamWriter stream(data);
    stream.setAutoFormatting(true);
    stream.writeStartDocument();

    stream.writeStartElement("pattern"); //start pattern
    //TODO: dont need to set the version when saving into a binary file.
    stream.writeAttribute("version", QString::number(SaveFile::Version_1_0));
    saveColors(&stream);
    saveCharts(&stream);
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

bool SaveFile::saveCustomStitches(QDataStream* stream)
{
    Q_UNUSED(stream);
    //TODO: save everything into a DataStream
    return true;
}

bool SaveFile::saveCharts(QXmlStreamWriter *stream)
{
    int tabCount = mTabWidget->count();
    
    for(int i = 0; i < tabCount; ++i) {
        stream->writeStartElement("chart"); //start chart
        ChartTab* tab = qobject_cast<ChartTab*>(mTabWidget->widget(i));
        if(!tab)
            continue;
        stream->writeTextElement("name", mTabWidget->tabText(i));
        
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
                stream->writeTextElement("color", c->color().name());
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

void SaveFile::saveColors(QXmlStreamWriter* stream)
{
    stream->writeStartElement("colors"); //start colors
    MainWindow *mw = static_cast<MainWindow*>(mParent);
    QMapIterator<QString, QMap<QString, int> > i(mw->patternColors());
    while (i.hasNext()) {
        i.next();
        stream->writeTextElement("color", i.key());
    }
    
    stream->writeEndElement(); // end colors
}

/*File Load:
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
            if(e.tagName() == "colors") {
                loadColors(&e);
            } else if(e.tagName() == "chart") {
                loadChart(&e);
            } else if (e.tagName() == "stitch") { // custom stitches.
                continue;
            }
        }
        n = n.nextSibling();
    }

    return SaveFile::No_Error;
}

bool SaveFile::loadCustomStitches(QDataStream* stream)
{
    Q_UNUSED(stream);
    //TODO: load everything from a DataStream
    return true;
}

void SaveFile::loadColors(QDomElement* element)
{
    MainWindow *mw = qobject_cast<MainWindow*>(mParent);
    
    QDomNode n = element->firstChild();
    while(!n.isNull()) {
        QDomElement e = n.toElement();
        if(!e.isNull()) {
            if(e.tagName() == "color") {
                QMap<QString, int> properties;
                properties.insert("color number", mw->patternColors().count() + 1);
                properties.insert("count", 0); //count = 0 because we haven't added any cells yet.
                mw->patternColors().insert(e.text(),properties);
            } else {
                qWarning() << "Cannot load unknown color property:" << e.tagName() << e.text();
            }
        }
        n = n.nextSibling();
    }
}

void SaveFile::loadChart(QDomElement *element)
{
    MainWindow *mw = qobject_cast<MainWindow*>(mParent);
    ChartTab* tab = mw->createTab();
    QString tabName;

    QDomNode n = element->firstChild();
    while(!n.isNull()) {
        QDomElement e = n.toElement();
        if(!e.isNull()) {
            if(e.tagName() == "name") {
                tabName = e.text();
            } else if(e.tagName() == "cell") {
                loadCell(tab, &e);
            } else {
                qWarning() << "Cannot load unknown stitch property:" << e.tagName() << e.text();
            }
        }
        n = n.nextSibling();
    }

    mTabWidget->addTab(tab, tabName);
}

void SaveFile::loadCell(ChartTab* tab, QDomElement *element)
{
    CrochetCell* c = new CrochetCell();
    int row, column;
    QString color;
    qreal x, y, rotation;
    double angle;
    
    QObject::connect(c, SIGNAL(stitchChanged(QString,QString)), tab->scene(), SIGNAL(stitchChanged(QString,QString)));
    QObject::connect(c, SIGNAL(colorChanged(QString,QString)), tab->scene(), SIGNAL(colorChanged(QString,QString)));
    
    QDomNode n = element->firstChild();
    while(!n.isNull()) {
        QDomElement e = n.toElement();
        if(!e.isNull()) {
            if(e.tagName() == "stitch") {
                Stitch *s = StitchCollection::inst()->findStitch(e.text());
                if(s)
                    c->setStitch(s);
            } else if(e.tagName() == "row") {
                row = e.text().toInt();
            } else if(e.tagName() == "column") {
                column = e.text().toInt();
            } else if(e.tagName() == "color") {
                color = e.text();
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

    //TODO: fix crash on appendCell...
    tab->scene()->appendCell(row, c);
    c->setColor(QColor(color));
    c->setPos(x, y);
    c->setRotation(rotation);
    c->setAngle(angle);
}
