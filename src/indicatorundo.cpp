/****************************************************************************\
 Copyright (c) 2011-2014 Stitch Works Software
 Brian C. Milco <bcmilco@gmail.com>

 This file is part of Crochet Charts.

 Crochet Charts is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Crochet Charts is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Crochet Charts. If not, see <http://www.gnu.org/licenses/>.

 \****************************************************************************/
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
	item->setLayer(scene->getCurrentLayer()->uid());
	item->setVisible(scene->getCurrentLayer()->visible());
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
