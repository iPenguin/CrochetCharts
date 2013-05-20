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
    setText(QObject::tr("change color"));
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
    setText(QObject::tr("change color"));
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
    setText(QObject::tr("change angle"));
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
SetSelectionRotation::SetSelectionRotation(Scene* s, QList<QGraphicsItem*> itms, qreal degrees,
                                           QUndoCommand* parent)
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

void SetSelectionRotation::redo()
{
    scene->rotateSelection(newAngle, items, pivotPoint);

}

void SetSelectionRotation::undo()
{
    scene->rotateSelection(-newAngle, items, pivotPoint);
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
    setText(QObject::tr("change position"));
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
    setText(QObject::tr("change scale"));
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
    setText(QObject::tr("add stitch"));

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
    setText(QObject::tr("group items"));

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
| RemoveGroup                                     |
\*************************************************/
RemoveGroup::RemoveGroup(Scene *scene, ItemGroup *group, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    s = scene;
    items = group->childItems();
    g = group;
    setText(QObject::tr("remove group"));
}

void RemoveGroup::redo()
{
    s->removeItem(g);
}

void RemoveGroup::undo()
{
    s->addItem(g);
    foreach(QGraphicsItem *i, g->childItems()) {
        i->setVisible(true);
    }
}
