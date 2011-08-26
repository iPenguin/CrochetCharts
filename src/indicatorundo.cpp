/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "indicatorundo.h"

#include <QDebug>
#include <QObject>

#include "indicator.h"
#include "scene.h"

/*************************************************\
| AddIndicator                                    |
\*************************************************/
AddIndicator::AddIndicator(Scene* s, QPointF pos, QUndoCommand* parent)
    : QUndoCommand(parent)
{
    position = pos;
    item = new Indicator();
    scene = s;

    setText(QObject::tr("Add indicator"));
}

void AddIndicator::redo()
{
    scene->addIndicator(item);
    item->setPos(position);
    item->setTextInteractionFlags(Qt::TextEditorInteraction);
}

void AddIndicator::undo()
{
    scene->removeIndicator(item);
}

/*************************************************\
| RemoveIndicator                                 |
\*************************************************/
RemoveIndicator::RemoveIndicator(Scene* s, Indicator* i, QUndoCommand* parent)
    : QUndoCommand(parent)
{
    item = i;
    scene = s;
    position = i->pos();
    setText(QObject::tr("Remove indicator"));
}

void RemoveIndicator::redo()
{
    scene->removeIndicator(item);
}

void RemoveIndicator::undo()
{
    scene->addIndicator(item);
    item->setPos(position);
    item->setTextInteractionFlags(Qt::TextEditorInteraction);
}

/*************************************************\
| MoveIndicator                                   |
\*************************************************/
MoveIndicator::MoveIndicator(Scene* s, Indicator* item, QPointF newPos, QUndoCommand* parent)
    : QUndoCommand(parent)
{
    i = item;
    origPosition = i->pos();
    newPosition = newPos;

    scene = s;

    setText(QObject::tr("Move indicator"));
}

void MoveIndicator::redo()
{
    i->setPos(newPosition);
}

void MoveIndicator::undo()
{
    i->setPos(origPosition);
}

bool MoveIndicator::mergeWith(const QUndoCommand* command)
{
    if(command->id() != id())
        return false;

    const MoveIndicator* other = static_cast<const MoveIndicator*>(command);

    Indicator* i = qgraphicsitem_cast<Indicator*>(scene->itemAt(origPosition));
    Indicator* otherI = qgraphicsitem_cast<Indicator*>(scene->itemAt(other->origPosition));

    if(otherI != i)
        return false;

    newPosition = other->newPosition;
    setText(QObject::tr("Move indicator"));
    return true;
}

/*************************************************\
| ChangeTextIndicator                             |
\*************************************************/
ChangeTextIndicator::ChangeTextIndicator(Scene* s, Indicator* item, QString text, QUndoCommand* parent)
    : QUndoCommand(parent)
{
    scene = s;
    i = item;

    origText = i->text();
    newText = text;

    setText(QObject::tr("Change text of indicator"));
}

void ChangeTextIndicator::redo()
{
    i->setText(newText);
}

void ChangeTextIndicator::undo()
{
    i->setText(origText);
}
