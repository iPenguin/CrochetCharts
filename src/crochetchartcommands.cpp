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
SetCellRotation::SetCellRotation(Scene *s, CrochetCell *cell, qreal baseRot, qreal diff, QUndoCommand* parent)
    : QUndoCommand(parent)
{
    scene = s;
    c = cell;
    delta = diff;
    baseRotation = baseRot;
    setText(QObject::tr("Change cell rotation"));
    
}

void SetCellRotation::redo()
{
    qreal final = baseRotation - delta;
    qreal pvtPt = c->stitch()->width()/2;
    c->setRotation(final, pvtPt);
}

void SetCellRotation::undo()
{
    qreal pvtPt = c->stitch()->width()/2;
    c->setRotation(baseRotation, pvtPt);
}

bool SetCellRotation::mergeWith(const QUndoCommand *command)
{
    if(command->id() != id())
        return false;
    //return false;
    const SetCellRotation *other = static_cast<const SetCellRotation*>(command);

    CrochetCell *otherC = other->c;
    
    if(otherC != c)
        return false;

    delta = other->delta;
    setText(QObject::tr("Change cell rotation"));
    return true;
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
SetCellScale::SetCellScale(Scene *s, CrochetCell *cell, qreal scl, QUndoCommand* parent)
    : QUndoCommand(parent)
{
    scene = s;
    c = cell;
    newScale = scl;
    oldScale = c->scale();
    setText(QObject::tr("Change cell scale"));
}

void SetCellScale::undo()
{
    c->setScale(1.0/newScale);
}

void SetCellScale::redo()
{
    c->setScale(newScale);
}

bool SetCellScale::mergeWith(const QUndoCommand *command)
{
    if(command->id() != id())
        return false;

    const SetCellScale *other = static_cast<const SetCellScale*>(command);
    
    CrochetCell *otherC = other->c;
    
    if(otherC != c)
        return false;
    
    newScale += other->newScale;

    setText(QObject::tr("Change cell scale"));
    return true;
}

/*************************************************\
| AddCell                                         |
\*************************************************/
AddCell::AddCell(Scene* s, QPoint pos, QUndoCommand* parent)
    : QUndoCommand(parent)
{

    position = pos;
    c = new CrochetCell();
    c->setColor();
    scene = s;
    setText(QObject::tr("Add cell"));

}

void AddCell::redo()
{
    scene->addCell(position, c);
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
    position = scene->findGridPosition(cell);
    setText(QObject::tr("Remove cell"));
}

void RemoveCell::redo()
{
    scene->removeCell(c);
}

void RemoveCell::undo()
{
    scene->addCell(position, c);
}

/*************************************************\
| AddRow                                          |
\*************************************************/
AddRow::AddRow(Scene* s, QPointF pos, QUndoCommand* parent)
    : QUndoCommand(parent)
{
    position = pos;
    scene = s;
    setText(QObject::tr("Add row"));
}

void AddRow::redo()
{

}

void AddRow::undo()
{

}

/*************************************************\
| RemoveRow                                       |
\*************************************************/
RemoveRow::RemoveRow(Scene* s, QPointF pos, QUndoCommand* parent)
    : QUndoCommand(parent)
{
    position = pos;
    scene = s;
    setText(QObject::tr("Remove row"));
}

void RemoveRow::redo()
{

}

void RemoveRow::undo()
{

}
