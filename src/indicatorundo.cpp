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
    scene->addItem(item);
    item->setPos(position);
    item->setTextInteractionFlags(Qt::TextEditorInteraction);
}

void AddIndicator::undo()
{
    scene->removeItem(item);
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
    scene->removeItem(item);
}

void RemoveIndicator::undo()
{
    scene->addItem(item);
    item->setPos(position);
    item->setTextInteractionFlags(Qt::TextEditorInteraction);
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
