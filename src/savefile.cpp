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

#include <QStringList>

#include <QDataStream>
#include "crochetscene.h"
#include "stitchlibrary.h"
#include "stitchset.h"

#include "indicator.h"

#include "savethread.h"

#include "mainwindow.h"
#include <QFileInfo>
#include <QDir>

SaveFile::SaveFile(QWidget* parent) :
    isSaved(false),
    fileName(""),
    mFileVersion(SaveFile::Version_1_0),
    mParent(parent),
    mInternalStitchSet(0)
{
    mTabWidget = static_cast<MainWindow*>(mParent)->tabWidget();
}

SaveFile::~SaveFile()
{
}

SaveFile::FileError SaveFile::save()
{
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly)) {
        //TODO: some nice dialog to warn the user.
        qWarning() << "Couldn't open file for writing..." << fileName;
        return SaveFile::Err_OpeningFile;
    }
    
    QDataStream out(&file);
    // Write a header with a "magic number" and a version
    out << AppInfo::inst()->magicNumber;
    out << (qint32)mFileVersion;

    out.setVersion(QDataStream::Qt_4_7);

    if(!mInternalStitchSet) {
        mInternalStitchSet = new StitchSet();
        mInternalStitchSet->isTemporary = true;
        StitchLibrary::inst()->addStitchSet(mInternalStitchSet);
    } else
        mInternalStitchSet->clearStitches();

    //start xml save...
    QString *data = new QString();
    QXmlStreamWriter stream(data);
    stream.setAutoFormatting(true);
    stream.writeStartDocument();

    stream.writeStartElement("pattern"); //start pattern
    //TODO: dont need to set the version when saving into a binary file.
    stream.writeAttribute("version", QString::number(SaveFile::Version_1_0));

    //create the StitchSet then save the icons.
    saveCustomStitches(&stream);
    mInternalStitchSet->saveIcons(&out);

    saveColors(&stream);
    saveCharts(&stream);
    stream.writeEndElement();
    
    stream.writeEndDocument();

    //put xml into binary file.
    out << data->toLatin1();
    file.close();
    delete data;
    data = 0;
    
    return SaveFile::No_Error;
}

void SaveFile::saveCustomStitches(QXmlStreamWriter* stream)
{
    CrochetTab* tab = qobject_cast<CrochetTab*>(mTabWidget->widget(0));

    //FIXME: fileName includes the whole path.
    mInternalStitchSet->setName(QString("[%1]").arg(QFileInfo(fileName).fileName()));
    QStringList stitches = tab->patternStitches()->keys();

    foreach(QString st, stitches) {
        Stitch *s = StitchLibrary::inst()->findStitch(st);
        mInternalStitchSet->addStitch(s->copy());
    }

    mInternalStitchSet->saveXmlStitchSet(stream, true);
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
        stream->writeTextElement("style", QString::number(tab->scene()->mStyle));
        Stitch *s = tab->scene()->mDefaultStitch;
        if(s)
            stream->writeTextElement("defaultStitch", s->name());

        stream->writeTextElement("showChartCenter", QString::number(tab->scene()->showChartCenter()));
        
        int rows = tab->scene()->rowCount();
        
        for(int row = 0; row < rows; ++row) {
            int cols = tab->scene()->columnCount(row);
            
            for(int col = 0; col < cols; ++col) {
                CrochetCell* c = tab->scene()->cell(row, col);
                if(!c)
                    continue;
                stream->writeStartElement("cell"); //start cell
                stream->writeTextElement("stitch", c->stitch()->name());
                stream->writeTextElement("row", QString::number(row));
                stream->writeTextElement("column", QString::number(col));
                stream->writeTextElement("color", c->color().name());
                stream->writeTextElement("x", QString::number(c->pos().x()));
                stream->writeTextElement("y", QString::number(c->pos().y()));
                stream->writeTextElement("anchor_x", QString::number(c->mAnchor.x()));
                stream->writeTextElement("anchor_y", QString::number(c->mAnchor.y()));
                
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

        foreach(Indicator *i, tab->scene()->indicators()) {
            stream->writeStartElement("indicator");

                stream->writeTextElement("x", QString::number(i->pos().x()));
                stream->writeTextElement("y", QString::number(i->pos().y()));
                stream->writeTextElement("text", i->text());
                stream->writeTextElement("textColor", i->textColor().name());
                stream->writeTextElement("bgColor", i->bgColor().name());

            stream->writeEndElement(); //end indicator
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


SaveFile::FileError SaveFile::load()
{
    QFile file(fileName);

    if(!file.open(QIODevice::ReadOnly)) {
        //TODO: some nice dialog to warn the user.
        qWarning() << "Couldn't open file for reading..." << fileName;
        return SaveFile::Err_OpeningFile;
    }

    QDataStream in(&file);

    quint32 magicNumber;
    qint32 version;
    in >> magicNumber;

    if(magicNumber != AppInfo::inst()->magicNumber) {
        //TODO: nice error message. not a set file.
        qWarning() << "This is not a pattern file";
        file.close();
        return SaveFile::Err_WrongFileType;
    }

    in >> version;

    if(version < SaveFile::Version_1_0) {
        //TODO: unknown version.
        qWarning() << "Unknown file version";
        file.close();
        return SaveFile::Err_UnknownFileVersion;
    }

    if(version > mFileVersion) {
        //TODO: unknown file version
        qWarning() << "This file was created with a newer version of the software.";
        file.close();
        return SaveFile::Err_UnknownFileVersion;
    }

    if(version == SaveFile::Version_1_0)
        in.setVersion(QDataStream::Qt_4_7);

    mInternalStitchSet = new StitchSet();
    mInternalStitchSet->isTemporary = true;
    mInternalStitchSet->stitchSetFileName = StitchLibrary::inst()->nextSetSaveFile();
    QString dest = mInternalStitchSet->stitchSetFileName;
    QFileInfo info(dest);
    QDir(info.path()).mkpath(info.path() + "/" + info.baseName());

    mInternalStitchSet->loadIcons(&in);

    QByteArray docData;
    in >> docData;
    
    QXmlStreamReader stream(docData);

    if(stream.hasError()) {
        qWarning() << "Error loading saved file: " << stream.errorString();
        return SaveFile::Err_GettingFileContents;
    }
    
    while (!stream.atEnd() && !stream.hasError()) {

        stream.readNext();
        if (stream.isStartElement()) {
            QString name = stream.name().toString();
            if(name == "colors")
                loadColors(&stream);
            else if(name == "chart")
                loadChart(&stream);
            else if(name == "stitch_set")
                mInternalStitchSet->loadXmlStitchSet(&stream, true);
        }
    }

    StitchLibrary::inst()->addStitchSet(mInternalStitchSet);

    return SaveFile::No_Error;
}

void SaveFile::loadColors(QXmlStreamReader *stream)
{
    MainWindow *mw = qobject_cast<MainWindow*>(mParent);

    mw->patternColors().clear();
    
    while(!(stream->isEndElement() && stream->name() == "colors")) {
        stream->readNext();
        QString tag = stream->name().toString();

        if (tag == "color") {
            QMap<QString, qint64> properties;
            properties.insert("count", 0); //count = 0 because we haven't added any cells yet.
            properties.insert("added", (qint64) stream->attributes().value("added").toString().toLongLong());
            mw->patternColors().insert(stream->readElementText(),properties);
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

    while(!(stream->isEndElement() && stream->name() == "chart")) {
        stream->readNext();
        QString tag = stream->name().toString();

        if(tag == "name") {
            tabName = stream->readElementText();
        } else if(tag == "style") {
            tab->scene()->mStyle = (CrochetScene::ChartStyle)stream->readElementText().toInt();
        } else if(tag == "showChartCenter") {
            tab->scene()->setShowChartCenter(stream->readElementText().toInt());
        } else if(tag == "defaultStitch") {
            Stitch *s = StitchLibrary::inst()->findStitch(stream->readElementText());
            if(s) {
                tab->scene()->mDefaultStitch = s;
            } else {
                tab->scene()->mDefaultStitch = StitchLibrary::inst()->findStitch("ch");
            }
        } else if(tag == "cell") {
            SaveThread *sth = new SaveThread(tab, stream);
            QThread bgThread;
            sth->moveToThread(&bgThread);
            bgThread.start();
            sth->run();
            bgThread.quit();
            bgThread.wait();
        } else if(tag == "indicator") {
            loadIndicator(tab, stream);
        }
    }

    int index = mTabWidget->indexOf(tab);
    mTabWidget->setTabText(index, tabName);
    mTabWidget->widget(mTabWidget->indexOf(tab))->show();
}

void SaveFile::loadIndicator(CrochetTab *tab, QXmlStreamReader *stream)
{
    Indicator *i = new Indicator();

    qreal x = 0, y = 0;
    QString textColor, bgColor;
    QString text;

    while(!(stream->isEndElement() && stream->name() == "indicator")) {
        stream->readNext();
        QString tag = stream->name().toString();

        if(tag == "x") {
            x = stream->readElementText().toDouble();
        } else if(tag == "y") {
            y = stream->readElementText().toDouble();
        } else if(tag == "text") {
            text = stream->readElementText();
        } else if(tag == "textColor") {
            textColor = stream->readElementText();
        } else if(tag == "bgColor") {
            bgColor = stream->readElementText();
        }
    }

    tab->scene()->addIndicator(i);
    i->setPos(x,y);
    i->setText(text);
    i->setTextColor(textColor);
    i->setBgColor(bgColor);
}

void SaveFile::cleanUp()
{
    if(mInternalStitchSet)
        StitchLibrary::inst()->removeSet(mInternalStitchSet);
}
