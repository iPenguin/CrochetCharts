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
#include "file_v2.h"

#include "debug.h"

#include "appinfo.h"

#include <QFileInfo>
#include <QDir>

#include "stitchlibrary.h"
#include "mainwindow.h"
#include "scene.h"
#include "settings.h"
#include <QStack>

#include "crochettab.h"

File_v2::File_v2(MainWindow *mw, FileFactory *parent)
    : File(mw, parent)
{

}

FileFactory::FileError File_v2::load(QDataStream *stream)
{

    mInternalStitchSet = new StitchSet();
    mInternalStitchSet->isTemporary = true;
    mInternalStitchSet->stitchSetFileName = StitchLibrary::inst()->nextSetSaveFile();
    QString dest = mInternalStitchSet->stitchSetFileName;

    QFileInfo info(dest);
    QDir(info.path()).mkpath(info.path() + "/" + info.baseName());

    mInternalStitchSet->loadIcons(stream);

    QByteArray docData;
    *stream >> docData;

    QXmlStreamReader xmlStream(docData);

    if(xmlStream.hasError()) {
        qWarning() << "Error loading saved file: " << xmlStream.errorString();
        return FileFactory::Err_GettingFileContents;
    }

    while (!xmlStream.atEnd() && !xmlStream.hasError()) {

        xmlStream.readNext();
        if (xmlStream.isStartElement()) {
            QString name = xmlStream.name().toString();

            if(name == "colors") {
                loadColors(&xmlStream);

            } else if(name == "chart") {
                loadChart(&xmlStream);

            } else if(name == "stitch_set") {
                mInternalStitchSet->loadXmlStitchSet(&xmlStream, true);
                StitchLibrary::inst()->addStitchSet(mInternalStitchSet);
            }
        }
    }

    return FileFactory::No_Error;
}

FileFactory::FileError File_v2::save(QDataStream *stream)
{

    *stream << (qint32)FileFactory::Version_1_2;
    stream->setVersion(QDataStream::Qt_4_7);

    if(!mInternalStitchSet) {

        mInternalStitchSet = new StitchSet();
        mInternalStitchSet->isTemporary = true;
        mInternalStitchSet->stitchSetFileName = StitchLibrary::inst()->nextSetSaveFile();
        StitchLibrary::inst()->addStitchSet(mInternalStitchSet);

    } else {
        mInternalStitchSet->clearStitches();
    }

    //start xml save...
    QString *data = new QString();
    QXmlStreamWriter xmlStream(data);
    xmlStream.setAutoFormatting(true);
    xmlStream.writeStartDocument();

    xmlStream.writeStartElement("pattern"); //start pattern
    //TODO: dont need to set the version when saving into a binary file.
    xmlStream.writeAttribute("version", QString::number(FileFactory::Version_1_2));

    //create the StitchSet then save the icons.
    saveCustomStitches(&xmlStream);
    mInternalStitchSet->saveIcons(stream);

    saveColors(&xmlStream);

    saveCharts(&xmlStream);
    xmlStream.writeEndElement();

    xmlStream.writeEndDocument();

    //put xml into binary file.
    *stream << data->toUtf8();

    delete data;
    data = 0;
	
	return FileFactory::No_Error;
}

void File_v2::loadColors(QXmlStreamReader *stream)
{

    MainWindow *mw = mMainWindow;

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

void File_v2::loadChart(QXmlStreamReader *stream)
{
    MainWindow *mw = mMainWindow;
    CrochetTab *tab = 0;
    QString tabName = "", defaultSt = "";

    while(!(stream->isEndElement() && stream->name() == "chart")) {
        stream->readNext();
        QString tag = stream->name().toString();

        if(tag == "name") {
            tabName = stream->readElementText();

        } else if(tag == "style") {
            int style = stream->readElementText().toInt();
            tab = mw->createTab((Scene::ChartStyle)style);

            mParent->mTabWidget->addTab(tab, "");
            mParent->mTabWidget->widget(mParent->mTabWidget->indexOf(tab))->hide();
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

            stream->readElementText(); //move to the next tag.
        } else if(tag == "cell") {
            loadCell(tab, stream);

        } else if(tag == "indicator") {
            loadIndicator(tab, stream);
		
        } else if(tag == "chartimage") {
            loadChartImage(tab, stream);
		
        } else if(tag == "group") {
            stream->readElementText().toInt();
            //create an empty group for future use.
            QList<QGraphicsItem*> items;
            tab->scene()->group(items);

        } else if(tag == "guidelines") {
            QString type = stream->attributes().value("type").toString();
            int rows = stream->attributes().value("rows").toString().toInt();
            int columns = stream->attributes().value("columns").toString().toInt();
            int cellHeight = stream->attributes().value("cellHeight").toString().toInt();
            int cellWidth = stream->attributes().value("cellWidth").toString().toInt();

            tab->scene()->mGuidelines.setType(type);
            tab->scene()->mGuidelines.setColumns(columns);
            tab->scene()->mGuidelines.setRows(rows);
            tab->scene()->mGuidelines.setCellWidth(cellWidth);
            tab->scene()->mGuidelines.setCellHeight(cellHeight);

            stream->readElementText(); //move to the next tag
            tab->scene()->updateGuidelines();
            emit tab->updateGuidelines(tab->scene()->guidelines());

        } else if (tag == "chartLayer") {
			QString name = stream->attributes().value("name").toString();
			unsigned int uid = stream->attributes().value("uid").toString().toUInt();
			bool visible = stream->attributes().value("visible").toString().toInt();
			tab->scene()->addLayer(name, uid);
			tab->scene()->getLayer(uid)->setVisible(visible);
			tab->scene()->selectLayer(uid);
            stream->readElementText(); //move to the next tag.
		} else if (tag == "size") {
			qreal x = stream->attributes().value("x").toString().toDouble();
			qreal y = stream->attributes().value("y").toString().toDouble();
			qreal width = stream->attributes().value("width").toString().toDouble();
			qreal height = stream->attributes().value("height").toString().toDouble();
			
			QRectF size = QRectF(x, y, width, height);
			tab->scene()->setSceneRect(size);
			
			stream->readElementText();
		} else {
            qWarning() << "loadChart Unknown tag:" << tag;
        }
    }
	
	//refresh the layers so the visibility and selectability of items is correct
	tab->scene()->refreshLayers();
		
    tab->updateRows();
    int index = mParent->mTabWidget->indexOf(tab);
    mParent->mTabWidget->setTabText(index, tabName);
    mParent->mTabWidget->widget(mParent->mTabWidget->indexOf(tab))->show();
    tab->scene()->updateSceneRect();
    if(tab->scene()->hasChartCenter()) {
        tab->view()->centerOn(tab->scene()->mCenterSymbol->sceneBoundingRect().center());
    } else {
        tab->view()->centerOn(tab->scene()->itemsBoundingRect().center());
    }
}

void File_v2::loadGrid(QXmlStreamReader *stream, Scene *scene)
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

void File_v2::loadIndicator(CrochetTab *tab, QXmlStreamReader *stream)
{
    Indicator *i = new Indicator();

    qreal x = 0, y = 0;
	bool fontused = false;
	int fontsize;
    QString textColor, bgColor, fontname;
    QString text, style;
    QTransform transform;
    qreal   m11 = 1, m12 = 0, m13 = 0,
            m21 = 0, m22 = 1, m23 = 0,
            m31 = 0, m32 = 0, m33 = 1;
    int group = -1;
	unsigned int layer = 0;
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
        } else if(tag == "style") {
            style = stream->readElementText();
        } else if(tag == "group") {
            group = stream->readElementText().toInt();
        } else if(tag == "transformation") {
            m11 = stream->attributes().value("m11").toString().toDouble();
            m12 = stream->attributes().value("m12").toString().toDouble();
            m13 = stream->attributes().value("m13").toString().toDouble();
            m21 = stream->attributes().value("m21").toString().toDouble();
            m22 = stream->attributes().value("m22").toString().toDouble();
            m23 = stream->attributes().value("m23").toString().toDouble();
            m31 = stream->attributes().value("m31").toString().toDouble();
            m32 = stream->attributes().value("m32").toString().toDouble();
            m33 = stream->attributes().value("m33").toString().toDouble();
            transform.setMatrix(m11, m12, m13, m21, m22, m23, m31, m32, m33);
            stream->readElementText();
        } else if (tag == "fontname") {
			fontname = stream->readElementText();
			fontused = true;
		} else if (tag == "fontsize") {
			fontsize = stream->readElementText().toInt();
			fontused = true;
		} else if (tag == "layer") {
			layer = stream->readElementText().toUInt();
		}
    }
	DEBUG("Style is: ");
	DEBUG(style);
    i->setTransform(transform);
    i->setPos(x,y);
    i->setText(text);
    i->setTextColor(textColor);
    i->setBgColor(bgColor);
	i->setLayer(layer);
	if (fontused)
		i->setFont(QFont(fontname, fontsize));
	
    tab->scene()->addItem(i);
    i->setTextInteractionFlags(Qt::TextEditorInteraction);

    if(style.isEmpty())
        style = Settings::inst()->value("chartRowIndicator").toString();
    i->setStyle(style);

    if(group != -1) {
        tab->scene()->addToGroup(group, i);
		tab->scene()->getGroup(group)->setLayer(layer);
	}
}

void File_v2::loadChartImage(CrochetTab* tab, QXmlStreamReader* stream)
{
    int group = -1;
    QPointF position(0.0,0.0);
    QPointF pivotPoint;
    qreal angle = 0.0;
    QPointF scale = QPointF(1.0,1.0);
    QTransform transform;
    qreal   m11 = 1, m12 = 0, m13 = 0,
            m21 = 0, m22 = 1, m23 = 0,
            m31 = 0, m32 = 0, m33 = 1;
	unsigned int layer = 0;
	QString filename;

    while(!(stream->isEndElement() && stream->name() == "chartimage")) {
        stream->readNext();
        QString tag = stream->name().toString();
		
		if(tag == "position") {
            position.rx() = stream->attributes().value("x").toString().toDouble();
            position.ry() = stream->attributes().value("y").toString().toDouble();
            stream->readElementText();

        } else if(tag == "angle") {
            angle = stream->readElementText().toDouble();

        } else if(tag == "scale") {
            scale.rx() = stream->attributes().value("x").toString().toDouble();
            scale.ry() = stream->attributes().value("y").toString().toDouble();
            stream->readElementText();

        } else if(tag == "pivotPoint") {
            pivotPoint.rx() = stream->attributes().value("x").toString().toDouble();
            pivotPoint.ry() = stream->attributes().value("y").toString().toDouble();
            stream->readElementText();

        } else if(tag == "group") {
            group = stream->readElementText().toInt();

        } else if(tag == "transformation") {
            m11 = stream->attributes().value("m11").toString().toDouble();
            m12 = stream->attributes().value("m12").toString().toDouble();
            m13 = stream->attributes().value("m13").toString().toDouble();
            m21 = stream->attributes().value("m21").toString().toDouble();
            m22 = stream->attributes().value("m22").toString().toDouble();
            m23 = stream->attributes().value("m23").toString().toDouble();
            m31 = stream->attributes().value("m31").toString().toDouble();
            m32 = stream->attributes().value("m32").toString().toDouble();
            m33 = stream->attributes().value("m33").toString().toDouble();
            transform.setMatrix(m11, m12, m13, m21, m22, m23, m31, m32, m33);
            stream->readElementText();
        } else if (tag == "layer") {
			layer = stream->readElementText().toUInt();
		} else if (tag == "filename") {
			filename = stream->readElementText();
		}
    }
    ChartImage *c = new ChartImage(filename);
	
	c->setLayer(layer);

    tab->scene()->addItem(c);

    c->setZValue(10);

    c->setTransform(transform);
    c->setPos(position);
    c->setTransformOriginPoint(pivotPoint);
    c->setRotation(angle);
    if(group != -1) {
        tab->scene()->addToGroup(group, c);
		tab->scene()->getGroup(group)->setLayer(layer);
	}
}

void File_v2::loadCell(CrochetTab *tab, QXmlStreamReader *stream)
{

    Cell *c = new Cell();
    Stitch *s = 0;
    int row = -1, column = -1;
    int group = -1;
    QString bgColor, color;
    QPointF position(0.0,0.0);
    QPointF pivotPoint;
    qreal angle = 0.0;
    QPointF scale = QPointF(1.0,1.0);
    QTransform transform;
    qreal   m11 = 1, m12 = 0, m13 = 0,
            m21 = 0, m22 = 1, m23 = 0,
            m31 = 0, m32 = 0, m33 = 1;
	unsigned int layer = 0;

    while(!(stream->isEndElement() && stream->name() == "cell")) {
        stream->readNext();
        QString tag = stream->name().toString();

        if(tag == "stitch") {
            QString st = stream->readElementText();
            s = StitchLibrary::inst()->findStitch(st, true);

        } else if(tag == "grid") {
            row = stream->attributes().value("row").toString().toDouble();
            column = stream->attributes().value("column").toString().toDouble();
            stream->readElementText();

        } else if(tag == "color") {
            color = stream->readElementText();

        } else if(tag == "bgColor") {
            bgColor = stream->readElementText();

        } else if(tag == "position") {
            position.rx() = stream->attributes().value("x").toString().toDouble();
            position.ry() = stream->attributes().value("y").toString().toDouble();
            stream->readElementText();

        } else if(tag == "angle") {
            angle = stream->readElementText().toDouble();

        } else if(tag == "scale") {
            scale.rx() = stream->attributes().value("x").toString().toDouble();
            scale.ry() = stream->attributes().value("y").toString().toDouble();
            stream->readElementText();

        } else if(tag == "pivotPoint") {
            pivotPoint.rx() = stream->attributes().value("x").toString().toDouble();
            pivotPoint.ry() = stream->attributes().value("y").toString().toDouble();
            stream->readElementText();

        } else if(tag == "group") {
            group = stream->readElementText().toInt();

        } else if(tag == "transformation") {
            m11 = stream->attributes().value("m11").toString().toDouble();
            m12 = stream->attributes().value("m12").toString().toDouble();
            m13 = stream->attributes().value("m13").toString().toDouble();
            m21 = stream->attributes().value("m21").toString().toDouble();
            m22 = stream->attributes().value("m22").toString().toDouble();
            m23 = stream->attributes().value("m23").toString().toDouble();
            m31 = stream->attributes().value("m31").toString().toDouble();
            m32 = stream->attributes().value("m32").toString().toDouble();
            m33 = stream->attributes().value("m33").toString().toDouble();
            transform.setMatrix(m11, m12, m13, m21, m22, m23, m31, m32, m33);
            stream->readElementText();
        } else if (tag == "layer") {
			layer = stream->readElementText().toUInt();
		}
    }
	
	c->setLayer(layer);

    tab->scene()->addItem(c);

    if(row > -1 && column > -1) {
        c->setStitch(s);
        tab->scene()->grid[row].replace(column, c);
        c->setZValue(100);
    } else {
        c->setStitch(s);
        c->setZValue(10);
    }

    c->setTransform(transform);
    c->setPos(position);
    c->setBgColor(QColor(bgColor));
    c->setColor(QColor(color));
    c->setTransformOriginPoint(pivotPoint);
    c->setRotation(angle);
    if(group != -1) {
        tab->scene()->addToGroup(group, c);
		tab->scene()->getGroup(group)->setLayer(layer);
	}
}


void File_v2::saveCustomStitches(QXmlStreamWriter *stream)
{
    CrochetTab *tab = qobject_cast<CrochetTab*>(mTabWidget->widget(0));

    //FIXME: fileName includes the whole path.
    mInternalStitchSet->setName(QString("[%1]").arg(QFileInfo(mParent->fileName).fileName()));
    QStringList stitches = tab->patternStitches()->keys();

    foreach(QString st, stitches) {
        Stitch *s = StitchLibrary::inst()->findStitch(st);
        if(s)
            mInternalStitchSet->addStitch(s);
    }

    mInternalStitchSet->saveXmlStitchSet(stream, true);
}

bool File_v2::saveCharts(QXmlStreamWriter *stream)
{
    int tabCount = mTabWidget->count();

    for(int i = 0; i < tabCount; ++i) {
        CrochetTab *tab = qobject_cast<CrochetTab*>(mTabWidget->widget(i));
        if(!tab)
            continue;

	
		//first, block al qt signals for performance
		tab->scene()->blockSignals(true);
	
        stream->writeStartElement("chart"); //start chart

        stream->writeTextElement("name", mTabWidget->tabText(i));

        stream->writeTextElement("style", QString::number(tab->mChartStyle));
        stream->writeTextElement("defaultSt", tab->scene()->mDefaultStitch);

		//write the chart size 
		stream->writeStartElement("size");
		stream->writeAttribute("x", QString::number(tab->scene()->sceneRect().x()));
		stream->writeAttribute("y", QString::number(tab->scene()->sceneRect().y()));
		stream->writeAttribute("width", QString::number(tab->scene()->sceneRect().width()));
		stream->writeAttribute("height", QString::number(tab->scene()->sceneRect().height()));
		stream->writeEndElement();
	
        bool showCenter = tab->scene()->showChartCenter();
        if(showCenter) {
            stream->writeStartElement("chartCenter");
            stream->writeAttribute("x", QString::number(tab->scene()->mCenterSymbol->scenePos().x()));
            stream->writeAttribute("y", QString::number(tab->scene()->mCenterSymbol->scenePos().y()));
            stream->writeEndElement(); //end chart center

        }

        Guidelines guidelines = tab->scene()->guidelines();
        if(guidelines.type() != "None") {
            stream->writeStartElement("guidelines");
            stream->writeAttribute("type", guidelines.type());

            stream->writeAttribute("rows", QString::number(guidelines.rows()));
            stream->writeAttribute("columns", QString::number(guidelines.columns()));
            stream->writeAttribute("cellWidth", QString::number(guidelines.cellWidth()));
            stream->writeAttribute("cellHeight", QString::number(guidelines.cellHeight()));
            stream->writeEndElement();
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
		
		foreach(ChartLayer* l, tab->scene()->layers()) {
			stream->writeStartElement("chartLayer");
			stream->writeAttribute("name", l->name());
			stream->writeAttribute("uid", QString::number(l->uid()));
			stream->writeAttribute("visible", QString::number(l->visible()));
			
			stream->writeEndElement();
		}

        foreach(ItemGroup *g, tab->scene()->mGroups) {
            stream->writeTextElement("group", QString::number(tab->scene()->mGroups.indexOf(g)));
        }

        foreach(QGraphicsItem *item, tab->scene()->items()) {

            Cell *c = qgraphicsitem_cast<Cell*>(item);
            if(!c)
                continue;

            stream->writeStartElement("cell"); //start cell
            stream->writeTextElement("stitch", c->stitch()->name());
			stream->writeTextElement("layer", QString::number(c->layer()));

            //if the stitch is on the grid save the grid position.
            QPoint pt = tab->scene()->indexOf(c);
            if(pt != QPoint(-1, -1)) {
                stream->writeStartElement("grid");
                stream->writeAttribute("row", QString::number(pt.y()));
                stream->writeAttribute("column", QString::number(pt.x()));
                stream->writeEndElement(); //grid
            }

            bool isGrouped = c->parentItem() ? true : false;
            ItemGroup *g = 0;
			QList<QGraphicsItem*> ungroupstack;
			
            if(isGrouped) {
                g = qgraphicsitem_cast<ItemGroup*>(c->parentItem());
                int groupNum = tab->scene()->mGroups.indexOf(g);
                stream->writeTextElement("group", QString::number(groupNum));

				//ungroup this and all possible childs
				ungroupstack.append(c);
				while (ungroupstack.last()->parentItem()
					&& ungroupstack.last()->parentItem()->type() == ItemGroup::Type) {
					ungroupstack.append(ungroupstack.last()->parentItem());
				}
				
                //ungroup the items so that we can
                //take an acurate position of each stitch.
				//TODO this destroys all groups 
				for (int i = ungroupstack.count() - 1 ; i >= 2 ; i--) {
					ItemGroup* ig = qgraphicsitem_cast<ItemGroup*>(ungroupstack[i]);
					tab->scene()->ungroup(ig);
				}

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
            if(isGrouped) {
				g->addToGroup(c);
			}

            stream->writeTextElement("color", c->color().name());
            stream->writeTextElement("bgColor", c->bgColor().name());
            stream->writeTextElement("angle", QString::number(c->rotation()));

            stream->writeStartElement("scale");
            stream->writeAttribute("x", QString::number(c->transform().m11()));
            stream->writeAttribute("y", QString::number(c->transform().m22()));
            stream->writeEndElement(); //end scale

            stream->writeStartElement("pivotPoint");
            stream->writeAttribute("x", QString::number(c->transformOriginPoint().x()));
            stream->writeAttribute("y", QString::number(c->transformOriginPoint().y()));
            stream->writeEndElement(); //end pivotPoint

            stream->writeEndElement(); //end cell
        }
		foreach(QGraphicsItem *item, tab->scene()->items()) {
				ChartImage* c = qgraphicsitem_cast<ChartImage*>(item);
				if (!c)
					continue;
				
				stream->writeStartElement("chartimage"); //start cell
				stream->writeTextElement("layer", QString::number(c->layer()));
				stream->writeTextElement("filename", c->filename());
				bool isGrouped = c->parentItem() ? true : false;
				ItemGroup *g = 0;
				if(isGrouped) {
					g = qgraphicsitem_cast<ItemGroup*>(c->parentItem());
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
				
				stream->writeTextElement("angle", QString::number(c->rotation()));

				stream->writeStartElement("scale");
				stream->writeAttribute("x", QString::number(c->transform().m11()));
				stream->writeAttribute("y", QString::number(c->transform().m22()));
				stream->writeEndElement(); //end scale

				stream->writeStartElement("pivotPoint");
				stream->writeAttribute("x", QString::number(c->transformOriginPoint().x()));
				stream->writeAttribute("y", QString::number(c->transformOriginPoint().y()));
				stream->writeEndElement(); //end pivotPoint

				stream->writeEndElement(); //end indicator
		}
        foreach(Indicator *i, tab->scene()->indicators()) {
            stream->writeStartElement("indicator");

                stream->writeTextElement("x", QString::number(i->scenePos().x()));
                stream->writeTextElement("y", QString::number(i->scenePos().y()));
                stream->writeTextElement("text", i->text());
                stream->writeTextElement("textColor", i->textColor().name());
                stream->writeTextElement("bgColor", i->bgColor().name());
                stream->writeTextElement("style", i->style());
				stream->writeTextElement("fontname", i->font().toString());
				stream->writeTextElement("fontsize", QString::number(i->font().pointSize()));
				stream->writeTextElement("layer", QString::number(i->layer()));
                bool isGrouped = i->parentItem() ? true : false;
                ItemGroup *g = 0;
                if(isGrouped) {
                    g = qgraphicsitem_cast<ItemGroup*>(i->parentItem());
                    int groupNum = tab->scene()->mGroups.indexOf(g);
                    stream->writeTextElement("group", QString::number(groupNum));

                    //ungroup the items so that we can
                    //take an acurate position of each stitch.
                    g->removeFromGroup(i);
                }

                stream->writeStartElement("transformation");
                QTransform trans = i->transform();

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
                    g->addToGroup(i);
                
            stream->writeEndElement(); //end indicator
        }

        stream->writeEndElement(); // end chart
		
		//and resume signals
		tab->scene()->blockSignals(false);
    }

    return true;
}

void File_v2::saveColors(QXmlStreamWriter *stream)
{
    stream->writeStartElement("colors"); //start colors
    MainWindow *mw = static_cast<MainWindow*>(mParent->mParent);

    QStringList keys = mw->patternColors().keys();

    foreach(QString key, keys) {
        stream->writeStartElement("color");
        stream->writeAttribute("added", QString::number(mw->patternColors().value(key).value("added")));
        stream->writeCharacters(key);
        stream->writeEndElement(); //end color
    }

    stream->writeEndElement(); // end colors
}


void File_v2::cleanUp()
{
    if(mInternalStitchSet)
        StitchLibrary::inst()->removeSet(mInternalStitchSet);
}
