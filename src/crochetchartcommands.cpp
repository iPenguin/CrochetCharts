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

#include <QDebug>
#include <QObject>

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
    newAngle = item->rotation();
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
    item->setTransformOriginPoint(pivot);
    item->setRotation(angle);
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
        pivotPoint = QPointF(itemsRect.center().x(), itemsRect.bottom());
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
    g->setTransformOriginPoint(pivotPoint);
    g->setRotation(newAngle);
    scene->destroyItemGroup(g);
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
SetItemScale::SetItemScale(QGraphicsItem *item, QPointF oldScle, QUndoCommand *parent)
    : QUndoCommand(parent)
{

    i = item;
    newScale = QPointF(i->transform().m11(), i->transform().m22());
    oldScale = oldScle;
    setText(QObject::tr("change item scale"));
}

void SetItemScale::undo()
{
    setScale(i, oldScale);
}

void SetItemScale::redo()
{
    setScale(i, newScale);
}

void SetItemScale::setScale(QGraphicsItem *item, QPointF scale)
{

    QPointF txScale = QPointF(scale.x() / item->transform().m11(),
                              scale.y() / item->transform().m22());

    item->setTransform(item->transform().scale(txScale.x(), txScale.y()));
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
