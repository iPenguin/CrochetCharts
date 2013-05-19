/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "crochetchartcommands.h"

#include <QDebug>
#include <QObject>

/*************************************************\
| SetCellStitch                                   |
\*************************************************/
SetCellStitch::SetCellStitch(Scene* s, Cell* cell, QString newSt, QUndoCommand* parent)
    : QUndoCommand(parent)
{
    scene = s;
    c = cell;
    oldStitch = c->name();
    newStitch = newSt;
    setText(QObject::tr("change stitch"));
}

void SetCellStitch::redo()
{
    QPoint pos = scene->indexOf(c);
    bool useAlt = false;
    if(pos.y() != -1)
       useAlt = (pos.y() % 2);
    c->setStitch(newStitch, useAlt);
}

void SetCellStitch::undo()
{
    QPoint pos = scene->indexOf(c);
    bool useAlt = false;
    if(pos.y() != -1)
       useAlt = (pos.y() % 2);
    c->setStitch(oldStitch, useAlt);
}

/*************************************************\
| SetCellBgColor                                    |
\*************************************************/
SetCellBgColor::SetCellBgColor(Scene* s, Cell* cell, QColor newCl, QUndoCommand* parent)
    : QUndoCommand(parent)
{
    scene = s;
    c = cell;
    oldColor = c->bgColor();
    newColor = newCl;
    setText(QObject::tr("change color"));
}

void SetCellBgColor::redo()
{
    c->setBgColor(newColor);
}

void SetCellBgColor::undo()
{
    c->setBgColor(oldColor);
}

/*************************************************\
| SetCellColor                                    |
\*************************************************/
SetCellColor::SetCellColor(Scene* s, Cell* cell, QColor newCl, QUndoCommand* parent)
    : QUndoCommand(parent)
{
    scene = s;
    c = cell;
    oldColor = c->color();
    newColor = newCl;
    setText(QObject::tr("change color"));
}

void SetCellColor::redo()
{
    c->setColor(newColor);
}

void SetCellColor::undo()
{
    c->setColor(oldColor);
}

/*************************************************\
| SetItemRotation                                 |
\*************************************************/
SetItemRotation::SetItemRotation(Scene* s, QGraphicsItem* item, qreal oldAngl, QPointF pivotPt, QUndoCommand* parent)
    : QUndoCommand(parent)
{
    scene = s;
    i = item;
    oldAngle = oldAngl;
    newAngle = item->rotation();
    pvtPt = pivotPt;
    setText(QObject::tr("change angle"));
}

void SetItemRotation::redo()
{
    i->setTransformOriginPoint(pvtPt);
    i->setRotation(newAngle);
}

void SetItemRotation::undo()
{
    i->setRotation(oldAngle);
}

/*************************************************\
| SetItemsRotation                                |
\*************************************************/
SetItemsRotation::SetItemsRotation(Scene* s, QList<QGraphicsItem*> itms, qreal degrees, QUndoCommand* parent)
    : QUndoCommand(parent)
{
    scene = s;
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

void SetItemsRotation::redo()
{
    scene->rotateSelection(newAngle, items, pivotPoint);

}

void SetItemsRotation::undo()
{
    scene->rotateSelection(-newAngle, items, pivotPoint);
}

/*************************************************\
| SetItemCoordinates                              |
\*************************************************/
SetItemCoordinates::SetItemCoordinates(Scene* s, QGraphicsItem* item, QPointF oldPos, QUndoCommand* parent)
    : QUndoCommand(parent)
{
    scene = s;
    i = item;
    oldCoord = oldPos;
    newCoord = i->pos();
    setText(QObject::tr("change position"));
    //FIXME: use inverted() matrix to do scaling.
}

void SetItemCoordinates::undo()
{
    i->setPos(oldCoord);
}

void SetItemCoordinates::redo()
{
    i->setPos(newCoord);
}
 
/*************************************************\
 | SetItemScale                                   |
\*************************************************/
SetItemScale::SetItemScale(Scene* s, Item* item, QPointF oldScle, QUndoCommand* parent)
    : QUndoCommand(parent)
{
    scene = s;
    i = item;
    newScale = i->scale();
    oldScale = oldScle;
    setText(QObject::tr("change scale"));
}

void SetItemScale::undo()
{
    i->setScale(oldScale.x(), oldScale.y());
}

void SetItemScale::redo()
{
    i->setScale(newScale.x(), newScale.y());
}

/*************************************************\
| AddCell                                         |
\*************************************************/
AddCell::AddCell(Scene* s, QPointF pos, QUndoCommand* parent)
    : QUndoCommand(parent)
{

    position = pos;
    c = new Cell();
    scene = s;
    setText(QObject::tr("add stitch"));

}

void AddCell::redo()
{
    
    scene->addItem(c);
    c->setPos(position);
    
}

void AddCell::undo()
{
    scene->removeItem(c);
}

/*************************************************\
| RemoveItem                                      |
\*************************************************/
RemoveItem::RemoveItem(Scene* s, QGraphicsItem *i, QUndoCommand* parent)
    : QUndoCommand(parent)
{
    gi = i;
    scene = s;
    position = gi->pos();
    setText(QObject::tr("remove items"));
}

void RemoveItem::redo()
{
    scene->removeItem(gi);
}

void RemoveItem::undo()
{
    scene->addItem(gi);
    gi->setPos(position);
}

/*************************************************\
| GroupItems                                      |
\*************************************************/
GroupItems::GroupItems(Scene* s, QList<QGraphicsItem*> itemList, QUndoCommand* parent)
    : QUndoCommand(parent)
{
    scene = s;
    items = itemList;
    setText(QObject::tr("group items"));
    mGroup = 0;

}

void GroupItems::redo()
{
    mGroup = scene->group(items, mGroup);
}

void GroupItems::undo()
{
    scene->ungroup(mGroup);
}

/*************************************************\
| UngroupItems                                    |
\*************************************************/
UngroupItems::UngroupItems(Scene* s, ItemGroup* grp, QUndoCommand* parent)
    : QUndoCommand(parent)
{
    scene = s;
    group = grp;
    items = grp->childItems();
    setText(QObject::tr("group items"));

}

void UngroupItems::redo()
{
    scene->ungroup(group);
}

void UngroupItems::undo()
{
    group = scene->group(items, group);
}

/*************************************************\
| RemoveGroup                                     |
\*************************************************/
RemoveGroup::RemoveGroup(Scene* s, ItemGroup* grp, QUndoCommand* parent)
    : QUndoCommand(parent)
{
    scene = s;
    items = grp->childItems();
    group = grp;
    setText(QObject::tr("remove group"));
}

void RemoveGroup::redo()
{
    scene->removeItem(group);
}

void RemoveGroup::undo()
{
    scene->addItem(group);
    foreach(QGraphicsItem* i, group->childItems()) {
        i->setVisible(true);
    }
}
