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
SetCellStitch::SetCellStitch(Scene *s, CrochetCell *cell, QString newSt, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    scene = s;
    c = cell;
    oldStitch = c->name();
    newStitch = newSt;
    setText(QObject::tr("Change Stitch: %1 -> %2").arg(oldStitch).arg(newStitch));
}

void SetCellStitch::redo()
{
    QPoint pos = scene->findGridPosition(c);
    c->setStitch(newStitch, (pos.y() % 2));
}

void SetCellStitch::undo()
{
    QPoint pos = scene->findGridPosition(c);
    c->setStitch(oldStitch, (pos.y() % 2));
}

/*************************************************\
| SetCellColor                                    |
\*************************************************/
SetCellColor::SetCellColor(Scene *s, CrochetCell *cell, QColor newCl, QUndoCommand* parent)
    : QUndoCommand(parent)
{
    scene = s;
    c = cell;
    oldColor = c->color();
    newColor = newCl;
    setText(QObject::tr("Change Color: %1 -> %2").arg(oldColor.name()).arg(newColor.name()));
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
SetCellRotation::SetCellRotation(Scene *s, CrochetCell *cell, qreal oldAngl, QPointF pivotPt, QUndoCommand* parent)
    : QUndoCommand(parent)
{
    scene = s;
    c = cell;
    oldAngle = oldAngl;
    newAngle = cell->angle();
    pvtPt = pivotPt;
    scale = c->scale();
    setText(QObject::tr("Change cell rotation"));
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
| SetItemCoordinates                              |
\*************************************************/
SetItemCoordinates::SetItemCoordinates(Scene *s, QGraphicsItem *item, QPointF oldPos, QPointF newPos, QUndoCommand* parent)
    : QUndoCommand(parent)
{
    scene = s;
    i = item;
    oldCoord = oldPos;
    newCoord = newPos;
    setText(QObject::tr("Change item position"));
    
}

void SetItemCoordinates::undo()
{
    i->setPos(oldCoord);
}

void SetItemCoordinates::redo()
{
    i->setPos(newCoord);
}

bool SetItemCoordinates::mergeWith(const QUndoCommand *command)
{
    if(command->id() != id())
        return false;
    
    const SetItemCoordinates *other = static_cast<const SetItemCoordinates*>(command);
    
    QGraphicsItem *otherI = other->i;
    
    if(otherI != i)
        return false;

    newCoord = other->newCoord;
    setText(QObject::tr("Change item position"));
    return true;
}
 
/*************************************************\
 | SetCellScale                                   |
\*************************************************/
SetCellScale::SetCellScale(Scene *s, CrochetCell *cell, qreal oldScle, QPointF pvtPt, QUndoCommand* parent)
    : QUndoCommand(parent)
{
    scene = s;
    c = cell;
    pivotPt = pvtPt;
    newScale = cell->scale();
    oldScale = oldScle;
    setText(QObject::tr("Change cell scale"));
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
    setText(QObject::tr("Add cell"));

}

void AddCell::redo()
{
    scene->addCell(c);
    c->setPos(position);
}

void AddCell::undo()
{
    scene->removeCell(c);
}

/*************************************************\
| RemoveCell                                      |
\*************************************************/
RemoveCell::RemoveCell(Scene* s, CrochetCell *cell, QUndoCommand* parent)
    : QUndoCommand(parent)
{
    c = cell;
    scene = s;
    position = c->pos();
    setText(QObject::tr("Remove cell"));
}

void RemoveCell::redo()
{
    scene->removeCell(c);
}

void RemoveCell::undo()
{
    scene->addCell(c);
    c->setPos(position);
}

/*************************************************\
| AddItem                                         |
\*************************************************/
AddItem::AddItem(Scene *s, QPointF pos, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    position = pos;
    scene = s;
    i->setPos(pos);
    setText(QObject::tr("Add cell"));
}

void AddItem::undo()
{
    scene->removeItem(i);
}

void AddItem::redo()
{
    scene->addItem(i);
}
