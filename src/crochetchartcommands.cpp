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
SetCellStitch::SetCellStitch(CrochetScene *s, QPoint pos, QString newSt, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    position = pos;
    scene = s;
    oldStitch = scene->cell(position)->name();
    newStitch = newSt;
    setText(QObject::tr("Change Stitch: %1 -> %2").arg(oldStitch).arg(newStitch));
}

void SetCellStitch::redo()
{
    scene->cell(position)->setStitch(newStitch, (position.y() % 2));
}

void SetCellStitch::undo()
{
    scene->cell(position)->setStitch(oldStitch, (position.y() % 2));
}

/*************************************************\
| SetCellColor                                    |
\*************************************************/
SetCellColor::SetCellColor(CrochetScene *s, QPoint pos, QColor newCl, QUndoCommand* parent)
    : QUndoCommand(parent)
{
    position = pos;
    scene = s;
    oldColor = scene->cell(position)->color();
    newColor = newCl;
    setText(QObject::tr("Change Color: %1 -> %2").arg(oldColor.name()).arg(newColor.name()));
}

void SetCellColor::redo()
{
    scene->cell(position)->setColor(newColor);
}

void SetCellColor::undo()
{
    scene->cell(position)->setColor(oldColor);
}

/*************************************************\
| SetCellRotation                                 |
\*************************************************/
SetCellRotation::SetCellRotation(CrochetScene *s, QPoint pos, qreal baseRot, qreal diff, QUndoCommand* parent)
    : QUndoCommand(parent)
{
    position = pos;
    scene = s;
    delta = diff;
    baseRotation = baseRot;
    setText(QObject::tr("Change cell rotation"));
    
}

void SetCellRotation::redo()
{
    qreal final = baseRotation - delta;
    
    scene->cell(position)->setTransform(QTransform().translate(32, 0).rotate(final).translate(-32, 0));
}

void SetCellRotation::undo()
{
    scene->cell(position)->setTransform(QTransform().translate(32, 0).rotate(baseRotation).translate(-32, 0));
}

bool SetCellRotation::mergeWith(const QUndoCommand *command)
{
    if(command->id() != id())
        return false;
    //return false;
    const SetCellRotation *other = static_cast<const SetCellRotation*>(command);

    CrochetCell *c = scene->cell(position);
    CrochetCell *otherC = scene->cell(other->position);
    
    if(otherC != c)
        return false;

    delta = other->delta;
    setText(QObject::tr("Change cell rotation"));
    return true;
}


/*************************************************\
| SetCellCoordinates*                             |
\*************************************************/
SetCellCoordinates::SetCellCoordinates(CrochetScene *s, QPoint pos, QPointF oldPos, QPointF newPos, QUndoCommand* parent)
    : QUndoCommand(parent)
{
    scene = s;
    position = pos;
    oldCoord = oldPos;
    newCoord = newPos;
    setText(QObject::tr("Change cell position"));
    
}

void SetCellCoordinates::undo()
{
    scene->cell(position)->setPos(oldCoord);
}

void SetCellCoordinates::redo()
{
    scene->cell(position)->setPos(newCoord);
}

bool SetCellCoordinates::mergeWith(const QUndoCommand *command)
{
    if(command->id() != id())
        return false;
    
    const SetCellCoordinates *other = static_cast<const SetCellCoordinates*>(command);
    
    CrochetCell *c = scene->cell(position);
    CrochetCell *otherC = scene->cell(other->position);
    
    if(otherC != c)
        return false;

    newCoord = other->newCoord;
    setText(QObject::tr("Change cell position"));
    return true;
}
 
/*************************************************\
 | SetCellScale                                   |
\*************************************************/
SetCellScale::SetCellScale(CrochetScene *s, QPoint pos, qreal scl, QUndoCommand* parent)
: QUndoCommand(parent)
{
    scene = s;
    position = pos;
    scale = scl;
    setText(QObject::tr("Change cell position"));
    
}

void SetCellScale::undo()
{
    QTransform trans = scene->cell(position)->transform().scale(1, scene->cell(position)->transform().m22() - scale);
    scene->cell(position)->setTransform(trans);
}

void SetCellScale::redo()
{
    qreal delta;
    QTransform t = scene->cell(position)->transform();
    if(scale > 0)
        delta = 0.05;
    else
        delta = -0.05;
    
    QTransform trans = scene->cell(position)->transform().scale(1, 1 + delta);
    scene->cell(position)->setTransform(trans);
}

bool SetCellScale::mergeWith(const QUndoCommand *command)
{
    if(command->id() != id())
        return false;
    
    const SetCellScale *other = static_cast<const SetCellScale*>(command);
    
    CrochetCell *c = scene->cell(position);
    CrochetCell *otherC = scene->cell(other->position);
    
    if(otherC != c)
        return false;
    
    scale += other->scale;
    setText(QObject::tr("Change cell position"));
    return true;
}
