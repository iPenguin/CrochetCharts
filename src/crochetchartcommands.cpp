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
    setText(QObject::tr("Change Stitch: %1 -> %2").arg(oldStitch).arg(newStitch));
}

void SetCellStitch::redo()
{
    c->setStitch(newStitch);
}

void SetCellStitch::undo()
{
    c->setStitch(oldStitch);
}

/*************************************************\
| SetCellColor                                    |
\*************************************************/
SetCellColor::SetCellColor(Cell* cell, QColor newCl, QUndoCommand* parent)
    : QUndoCommand(parent)
{
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
SetCellRotation::SetCellRotation(Cell* cell, qreal rot, QUndoCommand* parent)
    : QUndoCommand(parent)
{
    c = cell;
    rotation = rot;
    setText(QObject::tr("Change Position: %1").arg(rotation));
}

void SetCellRotation::redo()
{
    c->setTransform(QTransform().translate(32, 0).rotate(rotation).translate(-32, 0), true);
}

void SetCellRotation::undo()
{
    c->setTransform(QTransform().translate(32, 0).rotate(-rotation).translate(-32, 0), true);
}

bool SetCellRotation::mergeWith(const QUndoCommand* command)
{
    qDebug() << "mergeWith";
    if(command->id() != id())
        return false;

    const SetCellRotation *other = static_cast<const SetCellRotation*>(command);
    if(other->c != c)
        return false;
    
    rotation += other->rotation;
    setText(QObject::tr("Change Postion: %1").arg(rotation));
    return true;
}

