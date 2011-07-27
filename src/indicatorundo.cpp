/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "indicatorundo.h"

#include <QDebug>
#include <QObject>

#include "indicator.h"
#include "crochetscene.h"

/*************************************************\
| AddIndicator                                    |
\*************************************************/
AddIndicator::AddIndicator(CrochetScene *s, QPointF pos, QUndoCommand* parent)
    : QUndoCommand(parent)
{

    position = pos;
    scene = s;

    setText(QObject::tr("Add indicator"));
}

void AddIndicator::redo()
{
    Indicator *i = new Indicator(0, scene);
    scene->addIndicator(i);
    i->setPos(position);
    i->setTextInteractionFlags(Qt::TextEditorInteraction);
}

void AddIndicator::undo()
{
    scene->removeIndicator(position);
}

/*************************************************\
| RemoveIndicator                                 |
\*************************************************/
RemoveIndicator::RemoveIndicator(CrochetScene *s, Indicator *i, QUndoCommand* parent)
    : QUndoCommand(parent)
{
    position = i->pos();
    text = i->text();
    bgColor = i->bgColor();
    textColor = i->textColor();
    scene = s;

    setText(QObject::tr("Remove indicator"));
}

void RemoveIndicator::redo()
{
    scene->removeIndicator(position);
}

void RemoveIndicator::undo()
{
    Indicator *i = new Indicator(0, scene);
    scene->addIndicator(i);
    i->setPos(position);
    i->setText(text);
    i->setBgColor(bgColor);
    i->setTextColor(textColor);
    i->setTextInteractionFlags(Qt::TextEditorInteraction);
}

/*************************************************\
| MoveIndicator                                   |
\*************************************************/
MoveIndicator::MoveIndicator(CrochetScene* s, QPointF origPos, QPointF newPos, QUndoCommand* parent)
    : QUndoCommand(parent)
{
    origPosition = origPos;
    newPosition = newPos;

    scene = s;

    setText(QObject::tr("Move indicator"));
}

void MoveIndicator::redo()
{
    Indicator *i = qgraphicsitem_cast<Indicator*>(scene->itemAt(origPosition));
    if(!i) {
        qWarning() << "MoveIndicator::redo: Cannot find Indicator at " << origPosition;
        return;
    }
    i->setPos(newPosition);
}

void MoveIndicator::undo()
{
    Indicator *i = qgraphicsitem_cast<Indicator*>(scene->itemAt(newPosition));
    if(!i) {
        qWarning() << "MoveIndicator::undo: Cannot find Indicator at " << newPosition;
        return;
    }
    i->setPos(origPosition);
}

bool MoveIndicator::mergeWith(const QUndoCommand *command)
{
    if(command->id() != id())
        return false;

    const MoveIndicator *other = static_cast<const MoveIndicator*>(command);

    Indicator *i = qgraphicsitem_cast<Indicator*>(scene->itemAt(origPosition));
    Indicator *otherI = qgraphicsitem_cast<Indicator*>(scene->itemAt(other->origPosition));

    if(otherI != i)
        return false;

    newPosition = other->newPosition;
    setText(QObject::tr("Move indicator"));
    return true;
}

/*************************************************\
| ChangeTextIndicator                             |
\*************************************************/
ChangeTextIndicator::ChangeTextIndicator(CrochetScene* s, QPointF pos, QString text, QUndoCommand* parent)
    : QUndoCommand(parent)
{
    scene = s;
    position = pos;
    
    Indicator *i = qgraphicsitem_cast<Indicator*>(scene->itemAt(position));
    if(!i) {
        qWarning() << "ChangeTextIndicator::ChangeTextIndicator: Cannot find Indicator at " << position;
        return;
    }
    origText = i->text();
    newText = text;

    setText(QObject::tr("Change text of indicator"));
}

void ChangeTextIndicator::redo()
{
    Indicator *i = qgraphicsitem_cast<Indicator*>(scene->itemAt(position));
    if(!i) {
        qWarning() << "ChangeTextIndicator::redo: Cannot find Indicator at " << position;
        return;
    }
    i->setText(newText);
}

void ChangeTextIndicator::undo()
{
    Indicator *i = qgraphicsitem_cast<Indicator*>(scene->itemAt(position));
    if(!i) {
        qWarning() << "ChangeTextIndicator::undo: Cannot find Indicator at " << position;
        return;
    }
    i->setText(origText);
}
