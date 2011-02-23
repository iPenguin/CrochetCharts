/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "savefile.h"
#include "appinfo.h"

#include "crochettab.h"

#include <QDebug>

#include <QFile>

#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include <QDataStream>
#include "crochetscene.h"
#include "stitchlibrary.h"
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
        CrochetTab* tab = qobject_cast<CrochetTab*>(mTabWidget->widget(i));
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

                stream->writeStartElement("transformation");
                    QTransform trans = c->transform();
                    stream->writeTextElement("m11", QString::number(trans.m11()));
                    stream->writeTextElement("m12", QString::number(trans.m12()));
                    stream->writeTextElement("m13", QString::number(trans.m13()));
                    stream->writeTextElement("m21", QString::number(trans.m21()));
                    stream->writeTextElement("m22", QString::number(trans.m22()));
                    stream->writeTextElement("m23", QString::number(trans.m23()));
                    stream->writeTextElement("m31", QString::number(trans.m31()));
                    stream->writeTextElement("m32", QString::number(trans.m32()));
                    stream->writeTextElement("m33", QString::number(trans.m33()));
                stream->writeEndElement(); //transformation
                
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

    QStringList keys = mw->patternColors().keys();
    
    foreach(QString key, keys) {
        stream->writeStartElement("color");
        stream->writeAttribute("added", QString::number(mw->patternColors().value(key).value("added")));
        stream->writeCharacters(key);
        stream->writeEndElement(); //end color
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
    
    QXmlStreamReader stream(&file);

    if(stream.hasError()) {
        qWarning() << "Error loading saved file: " << stream.errorString();
        return SaveFile::Err_GettingFileContents;
    }

    while (!stream.atEnd() && !stream.hasError())
    {
        stream.readNext();
        if (stream.isStartElement()) {
            QString name = stream.name().toString();
            if (name == "colors")
                loadColors(&stream);
            else if(name == "chart")
                //loadChart(&stream);
                stream.skipCurrentElement();
            else if (name == "stitches") //custom stitches
                continue;
        }
    }

    if (stream.hasError()) {
        qWarning() << "XML error: " << stream.errorString() << endl;
    } else if (stream.atEnd()) {
        qWarning() << "Reached end, done" << endl;
    }

    return SaveFile::No_Error;
}

bool SaveFile::loadCustomStitches(QDataStream* stream)
{
    Q_UNUSED(stream);
    //TODO: load everything from a DataStream
    return true;
}

void SaveFile::loadColors(QXmlStreamReader *stream)
{
    MainWindow *mw = qobject_cast<MainWindow*>(mParent);

    mw->patternColors().clear();
    
    while(!(stream->tokenType() == QXmlStreamReader::EndElement && stream->name() == "color")) {
        stream->readNext();
        QString tag = stream->name().toString();
        qDebug() << tag;
        if (tag == "color") {
            QMap<QString, qint64> properties;
            properties.insert("count", 0); //count = 0 because we haven't added any cells yet.
            properties.insert("added", (qint64) stream->attributes().value("added").toString().toLongLong());
            mw->patternColors().insert(stream->readElementText(),properties);
        } else {
            qWarning() << "Cannot load unknown color property:" << stream->name();
        }
    }
}

void SaveFile::loadChart(QXmlStreamReader* stream)
{
    MainWindow *mw = qobject_cast<MainWindow*>(mParent);
    CrochetTab* tab = mw->createTab();
    QString tabName;

    mTabWidget->addTab(tab, "");

    mTabWidget->widget(mTabWidget->indexOf(tab))->hide();
    
    while(!(stream->tokenType() == QXmlStreamReader::EndElement && stream->name() == "chart")) {
        stream->readNext();
        QString tag = stream->name().toString();
        qDebug() << tag;
        if(tag == "name") {
            tabName = stream->readElementText();
        } else if(tag == "cell") {
            loadCell(tab, stream);
        } else {
            qWarning() << "Cannot load unknown stitch property" << tag;
        }
    }
    
    int index = mTabWidget->indexOf(tab);
    mTabWidget->setTabText(index, tabName);
    mTabWidget->widget(mTabWidget->indexOf(tab))->show();
}

void SaveFile::loadCell(CrochetTab* tab, QXmlStreamReader* stream)
{
    CrochetCell* c = new CrochetCell();
    int row, column;
    QString color;
    qreal x, y;
    QTransform transform;
    double angle;
    
    QObject::connect(c, SIGNAL(stitchChanged(QString,QString)), tab->scene(), SIGNAL(stitchChanged(QString,QString)));
    QObject::connect(c, SIGNAL(colorChanged(QString,QString)), tab->scene(), SIGNAL(colorChanged(QString,QString)));
qDebug() << "loadCell" << stream->name() << stream->text();
    while(!(stream->tokenType() == QXmlStreamReader::EndElement && stream->name() == "chart")) {
        stream->readNext();
        QString tag = stream->name().toString();
        qDebug() << tag;
        if(tag == "stitch") {
            QString st = stream->readElementText();
            qDebug() << "stitch: " << st;
            Stitch *s = StitchLibrary::inst()->findStitch(st);
            if(s)
                c->setStitch(s);
        } else if(tag == "row") {
            row = stream->readElementText().toInt();
        } else if(tag == "column") {
            column = stream->readElementText().toInt();
        } else if(tag == "color") {
            color = stream->readElementText();
        } else if(tag == "x") {
            x = stream->readElementText().toInt();
        } else if(tag == "y") {
            y = stream->readElementText().toInt();
        } else if(tag == "transformation") {
            transform = loadTransform(stream);
        } else if(tag == "angle") {
           angle = stream->readElementText().toDouble();
        } else {
            qWarning() << "Cannot load unknown stitch property" << stream->name() << stream->text().toString();
        }
    }
    
    tab->scene()->appendCell(row, c, true);
    c->setColor(QColor(color));
    c->setPos(x, y);
    c->setTransform(transform);
    c->setAngle(angle);
}

QTransform SaveFile::loadTransform(QXmlStreamReader* stream)
{
    QTransform transform;

    qreal m11, m12, m13,
          m21, m22, m23,
          m31, m32, m33;

    while(!(stream->tokenType() == QXmlStreamReader::EndElement && stream->name() == "chart")) {
        stream->readNext();
        QString tag = stream->name().toString();
        qDebug() << "tag" << tag;
        if(tag == "m11") {
            m11 = (qreal)stream->readElementText().toDouble();
        } else if(tag == "m12") {
            m12 = (qreal)stream->readElementText().toDouble();
        } else if(tag == "m13") {
            m13 = (qreal)stream->readElementText().toDouble();
        } else if(tag == "m21") {
            m21 = (qreal)stream->readElementText().toDouble();
        } else if(tag == "m22") {
            m22 = (qreal)stream->readElementText().toDouble();
        } else if(tag == "m23") {
            m23 = (qreal)stream->readElementText().toDouble();
        } else if(tag == "m31") {
            m31 = (qreal)stream->readElementText().toDouble();
        } else if(tag == "m32") {
            m32 = (qreal)stream->readElementText().toDouble();
        } else if(tag == "m33") {
            m33 = (qreal)stream->readElementText().toDouble();
        } else {
            qWarning() << "Cannot load unknown transform property:" << stream->name();
        }
    }
    
    transform.setMatrix(m11, m12, m13, m21, m22, m23, m31, m32, m33);
    return transform;
}