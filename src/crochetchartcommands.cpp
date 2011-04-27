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
SetCellRotation::SetCellRotation(CrochetScene *s, QPoint pos, QPointF fst, QPointF snd, qreal baseRot, QUndoCommand* parent)
    : QUndoCommand(parent)
{
    position = pos;
    scene = s;
    first = fst;
    second = snd;
    baseRotation = baseRot;
    setText(QObject::tr("Change cell rotation"));
    
}

void SetCellRotation::redo()
{

    QPointF origin = scene->cell(position)->mapToScene(32, 0);
    
    QPointF rel1 = QPointF(first.x() - origin.x(), first.y() - origin.y());
    QPointF rel2 = QPointF(second.x() - origin.x(), second.y() - origin.y());
    qreal angle1 = scene->scenePosToAngle(rel1);
    qreal angle2 = scene->scenePosToAngle(rel2);
    
    qreal final = baseRotation - (angle1 - angle2);
    
    scene->cell(position)->setTransform(QTransform().translate(32, 0).rotate(final).translate(-32, 0));
}

void SetCellRotation::undo()
{
    QPointF origin = scene->cell(position)->mapToScene(32, 0);
    
    QPointF rel1 = QPointF(first.x() - origin.x(), first.y() - origin.y());
    QPointF rel2 = QPointF(second.x() - origin.x(), second.y() - origin.y());
    qreal angle1 = scene->scenePosToAngle(rel1);
    qreal angle2 = scene->scenePosToAngle(rel2);
    
    qreal final = baseRotation - (angle1 - angle2);
    
    scene->cell(position)->setTransform(QTransform().translate(32, 0).rotate(final).translate(-32, 0));
}

bool SetCellRotation::mergeWith(const QUndoCommand *command)
{
    if(command->id() != id())
        return false;
    
    const SetCellRotation *other = static_cast<const SetCellRotation*>(command);

    CrochetCell *c = scene->cell(position);
    CrochetCell *otherC = scene->cell(other->position);
    
    if(otherC != c)
        return false;

    baseRotation = other->baseRotation;
    first = other->first;
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
