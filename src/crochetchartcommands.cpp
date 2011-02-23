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
    scene->cell(position)->setStitch(newStitch);
}

void SetCellStitch::undo()
{
    scene->cell(position)->setStitch(oldStitch);
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
SetCellRotation::SetCellRotation(CrochetScene *s, QPoint pos, qreal rot, QUndoCommand* parent)
    : QUndoCommand(parent)
{
    position = pos;
    scene = s;
    rotation = rot;
    setText(QObject::tr("Change Position: %1").arg(rotation));
}

void SetCellRotation::redo()
{
    scene->cell(position)->setTransform(QTransform().translate(32, 0).rotate(rotation).translate(-32, 0), true);
}

void SetCellRotation::undo()
{
    scene->cell(position)->setTransform(QTransform().translate(32, 0).rotate(-rotation).translate(-32, 0), true);
}

bool SetCellRotation::mergeWith(const QUndoCommand *command)
{
    if(command->id() != id())
        return false;
    
    const SetCellRotation *other = static_cast<const SetCellRotation*>(command);

    Cell *c = scene->cell(position);
    Cell *otherC = scene->cell(other->position);
    
    if(otherC != c)
        return false;
    
    rotation += other->rotation;
    setText(QObject::tr("Change Postion: %1").arg(rotation));
    return true;
}

