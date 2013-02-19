/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "savefile.h"
#include "appinfo.h"

#include "crochettab.h"

#include "debug.h"

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDataStream>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include <QStringList>

#include "scene.h"
#include "stitchlibrary.h"
#include "stitchset.h"

#include "indicator.h"
#include "savethread.h"
#include "mainwindow.h"

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
    //Don't save a file without at least 1 tab.
    if(mTabWidget->count() <= 0)
        return SaveFile::Err_NoTabsToSave;
    
    QFile file(fileName + ".tmp");
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
        mInternalStitchSet->stitchSetFileName = StitchLibrary::inst()->nextSetSaveFile();
        StitchLibrary::inst()->addStitchSet(mInternalStitchSet);
        
    } else {
        mInternalStitchSet->clearStitches();
    }

    //start xml save...
    QString* data = new QString();
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
    out << data->toUtf8();
    file.close();
    delete data;
    data = 0;

    QDir d(QFileInfo(fileName).path());
    //only try to delete the file if it exists.
    if(d.exists(fileName)) {
        if(!d.remove(fileName))
            return SaveFile::Err_RemovingOrigFile;
    }
    
    if(!d.rename(fileName +".tmp", fileName))
        return SaveFile::Err_RenamingTempFile;

    return SaveFile::No_Error;
}

void SaveFile::saveCustomStitches(QXmlStreamWriter* stream)
{
    CrochetTab* tab = qobject_cast<CrochetTab*>(mTabWidget->widget(0));

    //FIXME: fileName includes the whole path.
    mInternalStitchSet->setName(QString("[%1]").arg(QFileInfo(fileName).fileName()));
    QStringList stitches = tab->patternStitches()->keys();

    foreach(QString st, stitches) {
        Stitch* s = StitchLibrary::inst()->findStitch(st);
        if(s)
            mInternalStitchSet->addStitch(s);
    }

    mInternalStitchSet->saveXmlStitchSet(stream, true);
}

bool SaveFile::saveCharts(QXmlStreamWriter* stream)
{
    int tabCount = mTabWidget->count();
    
    for(int i = 0; i < tabCount; ++i) {
        CrochetTab* tab = qobject_cast<CrochetTab*>(mTabWidget->widget(i));
        if(!tab)
            continue;

        stream->writeStartElement("chart"); //start chart

        stream->writeTextElement("name", mTabWidget->tabText(i));

        stream->writeTextElement("style", QString::number(tab->mChartStyle));
        stream->writeTextElement("defaultSt", tab->scene()->mDefaultStitch);

        bool showCenter = tab->scene()->showChartCenter();
        if(showCenter) {
            stream->writeStartElement("chartCenter");
            stream->writeAttribute("x", QString::number(tab->scene()->mCenterSymbol->scenePos().x()));
            stream->writeAttribute("y", QString::number(tab->scene()->mCenterSymbol->scenePos().y()));
            stream->writeEndElement(); //end chart center
            
        }

        stream->writeStartElement("rowSpacing");
        stream->writeAttribute("width", QString::number(tab->scene()->mDefaultSize.width()));
        stream->writeAttribute("height", QString::number(tab->scene()->mDefaultSize.height()));
        stream->writeEndElement(); //row spacing

        if(tab->scene()->rowCount() >= 1 && tab->scene()->maxColumnCount() >= 1) {
            stream->writeStartElement("grid");
            int rowCount = tab->scene()->rowCount();
            for(int i = 0; i < rowCount; ++i) {
                int colCount = tab->scene()->columnCount(i);
                stream->writeTextElement("row", QString::number(colCount)); //row, columns.
            }
            stream->writeEndElement(); //end grid.
        }

        foreach(QGraphicsItemGroup* g, tab->scene()->mGroups) {
            stream->writeTextElement("group", QString::number(tab->scene()->mGroups.indexOf(g)));
        }

        foreach(QGraphicsItem* item, tab->scene()->items()) {
            
            Cell* c = qgraphicsitem_cast<Cell*>(item);
            if(!c)
                continue;
            
            stream->writeStartElement("cell"); //start cell
            stream->writeTextElement("stitch", c->stitch()->name());

            //if the stitch is on the grid save the grid position.
            QPoint pt = tab->scene()->indexOf(c);
            if(pt != QPoint(-1, -1)) {
                stream->writeStartElement("grid");
                stream->writeAttribute("row", QString::number(pt.y()));
                stream->writeAttribute("column", QString::number(pt.x()));
                stream->writeEndElement(); //grid
            }

            bool isGrouped = c->parentItem() ? true : false;
            QGraphicsItemGroup* g = 0;
            if(isGrouped) {
                g = qgraphicsitem_cast<QGraphicsItemGroup*>(c->parentItem());
                int groupNum = tab->scene()->mGroups.indexOf(g);
                stream->writeTextElement("group", QString::number(groupNum));

                //ungroup the items so that we can
                //take an acurate position of each stitch.
                g->removeFromGroup(c);
            }

            stream->writeStartElement("position");
            stream->writeAttribute("x", QString::number(c->pos().x()));
            stream->writeAttribute("y", QString::number(c->pos().y()));
            stream->writeEndElement(); //position

            stream->writeStartElement("transformation");
            QTransform trans = c->transform();

            stream->writeAttribute("m11", QString::number(trans.m11()));
            stream->writeAttribute("m12", QString::number(trans.m12()));
            stream->writeAttribute("m13", QString::number(trans.m13()));
            stream->writeAttribute("m21", QString::number(trans.m21()));
            stream->writeAttribute("m22", QString::number(trans.m22()));
            stream->writeAttribute("m23", QString::number(trans.m23()));
            stream->writeAttribute("m31", QString::number(trans.m31()));
            stream->writeAttribute("m32", QString::number(trans.m32()));
            stream->writeAttribute("m33", QString::number(trans.m33()));
            stream->writeEndElement(); //transformation

            //in case we haven't closed the
            //application we need to regroup the items.
            if(isGrouped)
                g->addToGroup(c);
            
            stream->writeTextElement("color", c->color().name());
            stream->writeTextElement("angle", QString::number(c->rotation()));

            stream->writeStartElement("scale");
            stream->writeAttribute("x", QString::number(c->scale().x()));
            stream->writeAttribute("y", QString::number(c->scale().y()));
            stream->writeEndElement(); //end scale

            stream->writeStartElement("pivotPoint");
            stream->writeAttribute("x", QString::number(c->transformOriginPoint().x()));
            stream->writeAttribute("y", QString::number(c->transformOriginPoint().y()));
            stream->writeEndElement(); //end pivotPoint
            
            stream->writeEndElement(); //end cell
        }

        foreach(Indicator* i, tab->scene()->indicators()) {
            stream->writeStartElement("indicator");

                stream->writeTextElement("x", QString::number(i->scenePos().x()));
                stream->writeTextElement("y", QString::number(i->scenePos().y()));
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
    MainWindow* mw = static_cast<MainWindow*>(mParent);

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

            if(name == "colors") {
                loadColors(&stream);

            } else if(name == "chart") {
                loadChart(&stream);
                
            } else if(name == "stitch_set") {
                mInternalStitchSet->loadXmlStitchSet(&stream, true);

            }
        }
    }

    StitchLibrary::inst()->addStitchSet(mInternalStitchSet);

    return SaveFile::No_Error;
}

void SaveFile::loadColors(QXmlStreamReader* stream)
{
    MainWindow* mw = qobject_cast<MainWindow*>(mParent);

    mw->patternColors().clear();
    
    while(!(stream->isEndElement() && stream->name() == "colors")) {
        stream->readNext();
        QString tag = stream->name().toString();

        if (tag == "color") {
            QMap<QString, qint64> properties;
            properties.insert("count", 0); //count = 0 because we haven't added any cells yet.
            properties.insert("added", (qint64) stream->attributes().value("added").toString().toLongLong());
            mw->mPatternColors.insert(stream->readElementText(),properties);
        }
    }

}

void SaveFile::loadChart(QXmlStreamReader* stream)
{
    MainWindow* mw = qobject_cast<MainWindow*>(mParent);
    CrochetTab* tab = 0;
    QString tabName = "", defaultSt = "";

    while(!(stream->isEndElement() && stream->name() == "chart")) {
        stream->readNext();
        QString tag = stream->name().toString();

        if(tag == "name") {
            tabName = stream->readElementText();
            
        } else if(tag == "style") {
            int style = stream->readElementText().toInt();
            tab = mw->createTab((Scene::ChartStyle)style);

            mTabWidget->addTab(tab, "");
            mTabWidget->widget(mTabWidget->indexOf(tab))->hide();
        } else if(tag == "defaultSt") {
            defaultSt = stream->readElementText();
            tab->scene()->mDefaultStitch = defaultSt;
            
        } else if(tag == "chartCenter") {
            qreal x = stream->attributes().value("x").toString().toDouble();
            qreal y = stream->attributes().value("y").toString().toDouble();

            stream->readElementText();
            tab->blockSignals(true);
            tab->setShowChartCenter(true);
            tab->scene()->mCenterSymbol->setPos(x, y);
            tab->blockSignals(false);

        } else if(tag == "grid") {
            loadGrid(stream, tab->scene());
            
        } else if(tag == "rowSpacing") {
            qreal width = stream->attributes().value("width").toString().toDouble();
            qreal height = stream->attributes().value("height").toString().toDouble();
            tab->scene()->mDefaultSize.setHeight(height);
            tab->scene()->mDefaultSize.setWidth(width);
            
        } else if(tag == "cell") {
            SaveThread* sth = new SaveThread(tab, stream);
            QThread bgThread;
            sth->moveToThread(&bgThread);
            bgThread.start();
            sth->run();
            bgThread.quit();
            bgThread.wait();
            
        } else if(tag == "indicator") {
            loadIndicator(tab, stream);
            
        } else if(tag == "group") {
            stream->readElementText().toInt();
            //create an empty group for future use.
            QList<QGraphicsItem*> items;
            tab->scene()->group(items);
        }
    }

    tab->scene()->initDemoBackground();
    tab->updateRows();
    int index = mTabWidget->indexOf(tab);
    mTabWidget->setTabText(index, tabName);
    mTabWidget->widget(mTabWidget->indexOf(tab))->show();
    tab->scene()->updateSceneRect();
    
    if(tab->scene()->hasChartCenter()) {
        tab->view()->centerOn(tab->scene()->mCenterSymbol->sceneBoundingRect().center());
    } else {
        tab->view()->centerOn(tab->scene()->itemsBoundingRect().center());
    }
}

void SaveFile::loadGrid(QXmlStreamReader* stream, Scene* scene)
{
    while(!(stream->isEndElement() && stream->name() == "grid")) {
        stream->readNext();
        QString tag = stream->name().toString();

        if(tag == "row") {
            int cols = stream->readElementText().toInt();
            QList<Cell*> row;
            for(int i = 0; i < cols; ++i) {
                row.append(0);
            }
            scene->grid.append(row);
        }
    }
}

void SaveFile::loadIndicator(CrochetTab* tab, QXmlStreamReader* stream)
{
    Indicator* i = new Indicator();

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

    tab->scene()->addItem(i);
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
