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
SetCellStitch::SetCellStitch(Scene* s, CrochetCell* cell, QString newSt, QUndoCommand* parent)
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
| SetCellColor                                    |
\*************************************************/
SetCellColor::SetCellColor(Scene* s, CrochetCell* cell, QColor newCl, QUndoCommand* parent)
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
| SetCellRotation                                 |
\*************************************************/
SetCellRotation::SetCellRotation(Scene* s, CrochetCell* cell, qreal oldAngl, QPointF pivotPt, QUndoCommand* parent)
    : QUndoCommand(parent)
{
    scene = s;
    c = cell;
    oldAngle = oldAngl;
    newAngle = cell->angle();
    pvtPt = pivotPt;
    scale = c->scale();
    setText(QObject::tr("change angle"));
}

void SetCellRotation::redo()
{
    
    c->setRotation(newAngle, pvtPt);
}

void SetCellRotation::undo()
{
    c->setRotation(oldAngle, pvtPt);
    c->setScale(scale, pvtPt);
}

/*************************************************\
| SetItemRotation                                 |
\*************************************************/
SetItemRotation::SetItemRotation(Scene* s, QList<QGraphicsItem*> itms, qreal degrees, QUndoCommand* parent)
    : QUndoCommand(parent)
{
    scene = s;
    items.append(itms);
    newAngle = degrees;

    QGraphicsItemGroup* group = scene->createItemGroup(items);
    pivotPoint = group->boundingRect().bottomLeft();
    oldAngle = group->rotation();
    scene->destroyItemGroup(group);
    qDebug() <<  oldAngle << newAngle;
    setText(QObject::tr("change angle"));
}

void SetItemRotation::redo()
{

    QGraphicsItemGroup* group = scene->createItemGroup(items);
    group->setTransformOriginPoint(group->mapToScene(pivotPoint));
    group->setTransform(QTransform().translate(pivotPoint.x(), pivotPoint.y()).rotate(newAngle).translate(-pivotPoint.x(), -pivotPoint.y()));
    scene->destroyItemGroup(group);

}

void SetItemRotation::undo()
{

    QGraphicsItemGroup* group = scene->createItemGroup(items);
    group->setTransformOriginPoint(group->mapToScene(pivotPoint));
    group->setTransform(QTransform().translate(-pivotPoint.x(), -pivotPoint.y()).rotate(-newAngle).translate(pivotPoint.x(), pivotPoint.y()));
    scene->destroyItemGroup(group);
    
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
 | SetCellScale                                   |
\*************************************************/
SetCellScale::SetCellScale(Scene* s, CrochetCell* cell, qreal oldScle, QPointF pvtPt, QUndoCommand* parent)
    : QUndoCommand(parent)
{
    scene = s;
    c = cell;
    pivotPt = pvtPt;
    newScale = cell->scale();
    oldScale = oldScle;
    setText(QObject::tr("change scale"));
}

void SetCellScale::undo()
{
    c->setScale(oldScale, pivotPt);
}

void SetCellScale::redo()
{
    c->setScale(newScale, pivotPt);
}

/*************************************************\
| AddCell                                         |
\*************************************************/
AddCell::AddCell(Scene* s, QPointF pos, QUndoCommand* parent)
    : QUndoCommand(parent)
{

    position = pos;
    c = new CrochetCell();
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
| RemoveCell                                      |
\*************************************************/
RemoveCell::RemoveCell(Scene* s, CrochetCell* cell, QUndoCommand* parent)
    : QUndoCommand(parent)
{
    c = cell;
    scene = s;
    position = c->pos();
    setText(QObject::tr("remove stitch"));
}

void RemoveCell::redo()
{
    scene->removeItem(c);
}

void RemoveCell::undo()
{
    scene->addItem(c);
    c->setPos(position);
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
UngroupItems::UngroupItems(Scene* s, QGraphicsItemGroup* grp, QUndoCommand* parent)
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
RemoveGroup::RemoveGroup(Scene* s, QGraphicsItemGroup* grp, QUndoCommand* parent)
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
