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
#include "crochetchartcommands.h"
#include "ChartItemTools.h"
#include "settings.h"
#include <QDebug>
#include <QObject>

/*************************************************\
| SetIndicatorText                                   |
\*************************************************/
SetIndicatorText::SetIndicatorText(Indicator *ind, QString otext, QString ntext, QUndoCommand *parent)
	: QUndoCommand(parent)
{
	i = ind;
	newText = ntext;
	oldText = otext;
    QUndoCommand::setText(QObject::tr("change indicator text"));
}

void SetIndicatorText::undo()
{
	setText(i, oldText);
}

void SetIndicatorText::redo()
{
	setText(i, newText);
}

void SetIndicatorText::setText(Indicator *i, QString text)
{
	i->setText(text);
}

/*************************************************\
| SetCellStitch                                   |
\*************************************************/
SetCellStitch::SetCellStitch(Cell *cell, QString newSt, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    c = cell;
    oldStitch = c->name();
    newStitch = newSt;
    setText(QObject::tr("change stitch"));
}

void SetCellStitch::redo()
{
    setStitch(c, newStitch);
}

void SetCellStitch::undo()
{
   setStitch(c, oldStitch);
}

void SetCellStitch::setStitch(Cell *cell, QString stitch)
{
    cell->setStitch(stitch);
}

/*************************************************\
| SetChartZLayer                                  |
\*************************************************/
SetChartZLayer::SetChartZLayer(ChartImage* image, const QString& layer, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    ci = image;
    newLayer = layer;
    oldLayer = ci->layer();
    setText(QObject::tr("change image layer"));
}

void SetChartZLayer::redo()
{
    setZLayer(ci, newLayer);
}

void SetChartZLayer::undo()
{
    setZLayer(ci, oldLayer);
}

void SetChartZLayer::setZLayer(ChartImage *ci, const QString& layer)
{
    ci->setZLayer(layer);
}

/*************************************************\
| SetChartImagePath                                  |
\*************************************************/
SetChartImagePath::SetChartImagePath(ChartImage* image, const QString& path, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    ci = image;
    newPath = path;
    oldPath = image->filename();
    setText(QObject::tr("change image path"));
}

void SetChartImagePath::redo()
{
    setPath(ci, newPath);
}

void SetChartImagePath::undo()
{
    setPath(ci, oldPath);
}

void SetChartImagePath::setPath(ChartImage *ci, const QString& path)
{
    ci->setFile(path);
}

/*************************************************\
| SetCellBgColor                                  |
\*************************************************/
SetCellBgColor::SetCellBgColor(Cell* cell, QColor newCl, QUndoCommand* parent)
    : QUndoCommand(parent)
{
    c = cell;
    oldColor = c->bgColor();
    newColor = newCl;
    setText(QObject::tr("change background color"));
}

void SetCellBgColor::redo()
{
    setBgColor(c, newColor);
}

void SetCellBgColor::undo()
{
    setBgColor(c, oldColor);
}

void SetCellBgColor::setBgColor(Cell *cell, QColor color)
{
    cell->setBgColor(color);
}

/*************************************************\
| SetCellColor                                    |
\*************************************************/
SetCellColor::SetCellColor(Cell *cell, QColor newCl, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    c = cell;
    oldColor = c->color();
    newColor = newCl;
    setText(QObject::tr("change stitch color"));
}

void SetCellColor::redo()
{
    setColor(c, newColor);
}

void SetCellColor::undo()
{
    setColor(c, oldColor);
}

void SetCellColor::setColor(Cell *cell, QColor color)
{
    cell->setColor(color);
}

/*************************************************\
| SetItemRotation                                 |
\*************************************************/
SetItemRotation::SetItemRotation(QGraphicsItem *item, qreal oldAngl, QPointF pivotPt, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    i = item;
    oldAngle = oldAngl;
    newAngle = ChartItemTools::getRotation(item);
    pvtPt = pivotPt;
    setText(QObject::tr("rotate item"));
}

void SetItemRotation::redo()
{
    setRotation(i, newAngle, pvtPt);
}

void SetItemRotation::undo()
{
    setRotation(i, oldAngle, pvtPt);
}

void SetItemRotation::setRotation(QGraphicsItem *item, qreal angle, QPointF pivot)
{
    //item->setTransformOriginPoint(pivot);
	ChartItemTools::setRotationPivot(item, pivot);
    ChartItemTools::setRotation(item, angle);
}

/*************************************************\
 | SetSelectionRotation                           |
\*************************************************/
SetSelectionRotation::SetSelectionRotation(Scene* scene, QList<QGraphicsItem*> itms, qreal degrees,
                                           QUndoCommand* parent)
    : QUndoCommand(parent)
{
    s = scene;
    items.append(itms);
    newAngle = degrees;

    if(scene->hasChartCenter()) {
        pivotPoint = scene->chartCenter()->sceneBoundingRect().center();
    } else {
        QRectF itemsRect = scene->selectedItemsBoundingRect(scene->selectedItems());
		if (Settings::inst()->value("rotateAroundCenter") == true) {
			pivotPoint = itemsRect.center() - QPointF(itemsRect.x(), itemsRect.y());
		} else {
			pivotPoint = QPointF(itemsRect.center().x(), itemsRect.bottom()) - QPointF(itemsRect.x(), itemsRect.y());
		}
    }

    setText(QObject::tr("rotate selection"));
}

void SetSelectionRotation::redo()
{
    rotate(s, newAngle, items, pivotPoint);

}

void SetSelectionRotation::undo()
{
    rotate(s, -newAngle, items, pivotPoint);
}

void SetSelectionRotation::rotate(Scene *scene, qreal degrees,
                                  QList<QGraphicsItem*> items, QPointF pivotPoint)
{

    qreal newAngle = degrees;
    qNormalizeAngle(newAngle);

    QGraphicsItemGroup *g = scene->createItemGroup(items);
	ChartItemTools::setRotationPivot(g, pivotPoint);
	ChartItemTools::setRotation(g, newAngle);
	QList<QGraphicsItem*> childs = g->childItems();
    scene->destroyItemGroup(g);
	foreach (QGraphicsItem* c, childs) {
		ChartItemTools::recalculateTransformations(c);
	}
}

/*************************************************\
| SetItemCoordinates                              |
\*************************************************/
SetItemCoordinates::SetItemCoordinates(QGraphicsItem *item, QPointF oldPos, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    i = item;
    oldCoord = oldPos;
    newCoord = i->pos();
    setText(QObject::tr("change item position"));
}

void SetItemCoordinates::undo()
{
    setPosition(i, oldCoord);
}

void SetItemCoordinates::redo()
{
    setPosition(i, newCoord);
}

void SetItemCoordinates::setPosition(QGraphicsItem *item, QPointF position)
{
    item->setPos(position);
}
 
/*************************************************\
 | SetItemScale                                   |
\*************************************************/
SetItemScale::SetItemScale(QGraphicsItem *item, QPointF oldScle, QPointF pivotPt, QUndoCommand *parent)
    : QUndoCommand(parent)
{
	mPivot = pivotPt;
    i = item;
    newScale = QPointF(ChartItemTools::getScaleX(i), ChartItemTools::getScaleY(i));
    oldScale = oldScle;
    setText(QObject::tr("change item scale"));
}

void SetItemScale::undo()
{
    setScale(i, oldScale, mPivot);
}

void SetItemScale::redo()
{
    setScale(i, newScale, mPivot);
}

void SetItemScale::setScale(QGraphicsItem *item, QPointF scale, QPointF pivot)
{
	ChartItemTools::setScalePivot(item, pivot);
	ChartItemTools::setScaleX(item, scale.x());
	ChartItemTools::setScaleY(item, scale.y());
}

/*************************************************\
| AddItem                                         |
\*************************************************/
AddItem::AddItem(Scene *scene, QGraphicsItem *item, QUndoCommand *parent)
    : QUndoCommand(parent)
{

    i = item;
    s = scene;
    setText(QObject::tr("add items"));
}

AddItem::~AddItem()
{
	//if the graphicsobject has no scene, delete it ourselves
	if (!i->scene())
		delete i;
}

void AddItem::redo()
{
    add(s, i);
}

void AddItem::undo()
{
    RemoveItem::remove(s, i);
}

void AddItem::add(Scene *scene, QGraphicsItem *item)
{
    scene->addItem(item);
}

/*************************************************\
| RemoveItem                                      |
\*************************************************/
RemoveItem::RemoveItem(Scene *scene, QGraphicsItem *item, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    i = item;
    s = scene;
    position = i->pos();
    setText(QObject::tr("remove items"));
}

void RemoveItem::redo()
{
    remove(s, i);
}

void RemoveItem::undo()
{
    AddItem::add(s, i);
}

void RemoveItem::remove(Scene *scene, QGraphicsItem *item)
{
    scene->removeItem(item);
}

/*************************************************\
| RemoveItems                                     |
\*************************************************/
RemoveItems::RemoveItems(Scene *scene, QList<QGraphicsItem*> i, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    items = i;
    s = scene;
    setText(QObject::tr("remove items"));
	removegroup = NULL;
}
RemoveItems::~RemoveItems()
{
	if (removegroup)
		delete removegroup;
}
void RemoveItems::redo()
{
	removegroup = s->createItemGroup(items);
    RemoveItem::remove(s, removegroup);
}

void RemoveItems::undo()
{
    AddItem::add(s, removegroup);
	//this call already deletes the itemgroup (according to the QT 4.8 doc. This may have changed in QT 5)
	s->destroyItemGroup(removegroup);
	removegroup = NULL;
}

/*************************************************\
| GroupItems                                      |
\*************************************************/
GroupItems::GroupItems(Scene *scene, QList<QGraphicsItem*> itemList, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    s = scene;
    items = itemList;
    setText(QObject::tr("group items"));
    g = 0;

}

void GroupItems::redo()
{
    g = s->group(items, g);
}

void GroupItems::undo()
{
    s->ungroup(g);
}

/*************************************************\
| UngroupItems                                    |
\*************************************************/
UngroupItems::UngroupItems(Scene *scene, ItemGroup *group, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    s = scene;
    g = group;
    items = group->childItems();
    setText(QObject::tr("ungroup items"));

}

void UngroupItems::redo()
{
    s->ungroup(g);
}

void UngroupItems::undo()
{
    g = s->group(items, g);
}

/*************************************************\
| AddLayer                                        |
\*************************************************/
AddLayer::AddLayer(Scene* scene, ChartLayer* layer, QUndoCommand* parent)
{
	s = scene;
	mLayer = layer;
    setText(QObject::tr("add layer"));
}

AddLayer::~AddLayer()
{
}
	
void AddLayer::undo()
{
	s->removeLayer(mLayer->uid());
	s->refreshLayers();
}
void AddLayer::redo()
{
	s->addLayer(mLayer);
	s->refreshLayers();
}

/*************************************************\
| RemoveLayer                                     |
\*************************************************/
RemoveLayer::RemoveLayer(Scene* scene, ChartLayer* layer, QUndoCommand* parent)
{
	s = scene;
	mLayer = layer;
    setText(QObject::tr("remove layer"));
}

void RemoveLayer::undo()
{
	s->addLayer(mLayer);
	s->refreshLayers();
}
void RemoveLayer::redo()
{
	s->removeLayer(mLayer->uid());
	s->refreshLayers();
}

/*************************************************\
| SetLayerStitch                                  |
\*************************************************/
SetLayerStitch::SetLayerStitch(Scene* scene, Cell* c, unsigned int layer, QUndoCommand* parent)
{
	s = scene;
	this->c = c;
	mNew = layer;
	mOld = c->layer();
	setText("set sitch layer");
}

void SetLayerStitch::undo()
{
	c->setLayer(mOld);
}

void SetLayerStitch::redo()
{
	c->setLayer(mNew);
}

/*************************************************\
| SetLayerIndicator                               |
\*************************************************/
SetLayerIndicator::SetLayerIndicator(Scene* scene, Indicator* c, unsigned int layer, QUndoCommand* parent)
{
	s = scene;
	this->c = c;
	mNew = layer;
	mOld = c->layer();
	setText("set indicator layer");
}

void SetLayerIndicator::undo()
{
	c->setLayer(mOld);
}

void SetLayerIndicator::redo()
{
	c->setLayer(mNew);
}

/*************************************************\
| SetLayerGroup                                   |
\*************************************************/
SetLayerGroup::SetLayerGroup(Scene* scene, ItemGroup* c, unsigned int layer, QUndoCommand* parent)
{
	s = scene;
	this->c = c;
	mNew = layer;
	mOld = c->layer();
	setText("set group layer");
}

void SetLayerGroup::undo()
{
	c->setLayer(mOld);
}

void SetLayerGroup::redo()
{
	c->setLayer(mNew);
}

/*************************************************\
| SetLayerimage                                |
\*************************************************/
SetLayerImage::SetLayerImage(Scene* scene, ChartImage* c, unsigned int layer, QUndoCommand* parent)
{
	s = scene;
	this->c = c;
	mNew = layer;
	mOld = c->layer();
	setText("set image layer");
}

void SetLayerImage::undo()
{
	c->setLayer(mOld);
}

void SetLayerImage::redo()
{
	c->setLayer(mNew);
}
