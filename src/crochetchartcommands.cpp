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
| SetCellCoordinates                              |
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

    if(scl > 0)
        delta = 0.075;
    else if(scl < 0)
        delta = -0.075;
    else
        delta = 0;
    deltas.append(delta);
    baseSize.append(1.0);
    
    setText(QObject::tr("Change cell position"));
}

void SetCellScale::undo()
{

    CrochetCell *c = scene->cell(position);

    qreal change = c->scale();
    for(int i = deltas.count() - 1; i >= 0; --i) {
        qreal value = deltas.at(i);
        qreal baseS = baseSize.at(i);
        change = (1.0 + value)/c->scale();
        qDebug() << c->scale() << 1.0 + value << change << baseS;
        c->setScale(change);
    }
}

void SetCellScale::redo()
{
    CrochetCell *c = scene->cell(position);

    foreach(qreal change, deltas) {
        c->setScale(c->scale() + change);
    }
}

bool SetCellScale::mergeWith(const QUndoCommand *command)
{
    if(command->id() != id())
        return false;
    //return false;
    const SetCellScale *other = static_cast<const SetCellScale*>(command);
    
    CrochetCell *c = scene->cell(position);
    CrochetCell *otherC = scene->cell(other->position);
    
    if(otherC != c)
        return false;

    if(other->delta != 0)
        deltas.append(other->delta);
        baseSize.append(other->baseSize);
    
    setText(QObject::tr("Change cell position"));
    return true;
}

/*************************************************\
| AddStitch                                       |
\*************************************************/
AddStitch::AddStitch(CrochetScene* s, QPoint pos, QUndoCommand* parent)
    : QUndoCommand(parent)
{
    position = pos;
    c = new CrochetCell();
    scene = s;
}

void AddStitch::redo()
{
    scene->insertCell(position, c);
}

void AddStitch::undo()
{
    scene->removeCell(c);
}

/*************************************************\
| RemoveStitch                                    |
\*************************************************/
RemoveStitch::RemoveStitch(CrochetScene* s, QPointF pos, QUndoCommand* parent)
    : QUndoCommand(parent)
{
    position = pos;
    scene = s;
}

void RemoveStitch::redo()
{
 
}

void RemoveStitch::undo()
{
 
}

/*************************************************\
| AddRow                                          |
\*************************************************/
AddRow::AddRow(CrochetScene* s, QPointF pos, QUndoCommand* parent)
    : QUndoCommand(parent)
{
    position = pos;
    scene = s;
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
RemoveRow::RemoveRow(CrochetScene* s, QPointF pos, QUndoCommand* parent)
    : QUndoCommand(parent)
{
    position = pos;
    scene = s;
}

void RemoveRow::redo()
{

}

void RemoveRow::undo()
{

}
