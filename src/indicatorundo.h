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
#ifndef INDICATORUNDO_H
#define INDICATORUNDO_H

#include <QUndoCommand>

class Indicator;
class Scene;

#include <QPointF>
#include <QString>
#include <QColor>

class AddIndicator : public QUndoCommand
{
public:
    enum { Id = 2200 };

    AddIndicator(Scene* s, QPointF pos, QUndoCommand* parent = 0);

    void undo();
    void redo();

    int id() const { return Id; }

private:
    QPointF position;

    Indicator* item;
    Scene* scene;
};

class RemoveIndicator : public QUndoCommand
{
public:
    enum { Id = 2210 };

    RemoveIndicator(Scene* s, Indicator* i, QUndoCommand* parent = 0);

    void redo();
    void undo();

    int id() const { return Id; }

private:
    QPointF position;
    Indicator* item;

    Scene* scene;
};


class ChangeTextIndicator : public QUndoCommand
{
public:
    enum { Id = 2230 };

    ChangeTextIndicator(Scene* s, Indicator* item, QString text, QUndoCommand* parent = 0);

    void redo();
    void undo();

    int id() const { return Id; }

private:
    Indicator* i;
    QString newText;
    QString origText;

    Scene* scene;
};

#endif //INDICATORUNDO_H
