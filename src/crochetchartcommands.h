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
#ifndef CROCHETCHARTCOMMANDS_H
#define CROCHETCHARTCOMMANDS_H

#include <QUndoCommand>

#include "cell.h"
#include "ChartImage.h"
#include "scene.h"

class SetIndicatorText : public QUndoCommand
{
public:
    enum { Id = 1100 };
    
    SetIndicatorText(Indicator *i, QString otext, QString ntext, QUndoCommand *parent = 0);

    void undo();
    void redo();

    int id() const { return Id; }
    
    static void setText(Indicator *i, QString text);

private:
    QString oldText;
    QString newText;
    Indicator* i;
};

class SetCellStitch : public QUndoCommand
{
public:
    enum { Id = 1100 };
    
    SetCellStitch(Cell *cell, QString newSt, QUndoCommand *parent = 0);

    void undo();
    void redo();

    int id() const { return Id; }
    
    static void setStitch(Cell *cell, QString stitch);

private:
    QString oldStitch;
    QString newStitch;
    Cell *c;
};

class SetChartImagePath : public QUndoCommand
{
public:
    enum { Id = 1110 };
	SetChartImagePath(ChartImage* ci, const QString& path, QUndoCommand *parent = 0);
	
	void undo();
    void redo();

    int id() const { return Id; }
    
    static void setPath(ChartImage *ci, const QString& path);
	
private:
	QString newPath;
	QString oldPath;
	ChartImage* ci;
};

class SetCellBgColor : public QUndoCommand
{
public:
    enum { Id = 1110 };
    
    SetCellBgColor(Cell *cell, QColor newCl, QUndoCommand *parent = 0);
    
    void undo();
    void redo();

    int id() const { return Id; }
    
    static void setBgColor(Cell *cell, QColor color);

private:
    QColor oldColor;
    QColor newColor;
    Cell *c;
};

class SetCellColor : public QUndoCommand
{
public:
    enum { Id = 1110 };

    SetCellColor(Cell *cell, QColor newCl, QUndoCommand *parent = 0);

    void undo();
    void redo();

    int id() const { return Id; }

    static void setColor(Cell *cell, QColor color);

private:
    QColor oldColor;
    QColor newColor;
    Cell *c;
};

class SetItemRotation : public QUndoCommand
{
public:
    enum { Id = 1120 };

    SetItemRotation(QGraphicsItem *item, qreal oldAngl, QPointF pivotPt, QUndoCommand *parent = 0);

    void undo();
    void redo();

    int id() const { return Id; }

    static void setRotation(QGraphicsItem *item, qreal angle, QPointF pivot);

private:
    QGraphicsItem *i;
    qreal oldAngle;
    qreal newAngle;
    QPointF pvtPt;

};

/**
 * No matter what the angle is convert it to a number between 0, and 360 degrees.
 */
static void qNormalizeAngle(qreal &angle)
{
    while (angle < 0.0)
        angle += 360.0;
    while (angle > 360.0)
        angle -= 360.0;
}

class SetSelectionRotation : public QUndoCommand
{
public:
    enum { Id = 1125 };

    SetSelectionRotation(Scene *scene, QList<QGraphicsItem*> itms, qreal degrees, QUndoCommand *parent = 0);

    void undo();
    void redo();

    int id() const { return Id; }

    static void rotate(Scene *scene, qreal degrees, QList<QGraphicsItem*> items, QPointF pivotPoint);

private:
    QList<QGraphicsItem*> items;
    qreal newAngle;

    QPointF pivotPoint;
    Scene *s;
};

class SetItemCoordinates : public QUndoCommand
{
public:
    enum { Id = 1130 };

    SetItemCoordinates(QGraphicsItem *item, QPointF oldPos, QUndoCommand *parent = 0);

    void undo();
    void redo();

    int id() const { return Id; }

    static void setPosition(QGraphicsItem *item, QPointF position);

private:
    QPointF oldCoord;
    QPointF newCoord;
    QGraphicsItem *i;
};

class SetItemScale : public QUndoCommand
{
public:
    enum { Id = 1140 };

    SetItemScale(QGraphicsItem *item, QPointF oldScle, QUndoCommand *parent = 0);

    void undo();
    void redo();

    int id() const { return Id; }

    static void setScale(QGraphicsItem *item, QPointF scale);

private:
    QPointF oldScale;
    QPointF newScale;

    QGraphicsItem *i;

};

class AddItem : public QUndoCommand
{
public:
    enum { Id = 1150 };

    AddItem(Scene *scene, QGraphicsItem *item, QUndoCommand *parent = 0);

    void redo();
    void undo();

    int id() const { return Id; }

    static void add(Scene *scene, QGraphicsItem *item);

private:
    QGraphicsItem *i;
    Scene *s;
};

class RemoveItem : public QUndoCommand
{
public:
    enum { Id = 1160 };

    RemoveItem(Scene *scene, QGraphicsItem *item, QUndoCommand *parent = 0);

    void redo();
    void undo();

    int id() const { return Id; }

    static void remove(Scene *scene, QGraphicsItem *item);

private:
    QGraphicsItem *i;
    QPointF position;

    Scene *s;
};

class GroupItems : public QUndoCommand
{
public:
    enum { Id = 1200 };

    GroupItems(Scene *scene, QList<QGraphicsItem*> itemList, QUndoCommand *parent = 0);

    void undo();
    void redo();

    int id() const { return Id; }

    ItemGroup* group() { return g; }

private:
    QList<QGraphicsItem*> items;
    ItemGroup *g;
    Scene *s;
};

class UngroupItems : public QUndoCommand
{
public:
    enum { Id = 1210 };

    UngroupItems(Scene *scene, ItemGroup *group, QUndoCommand *parent = 0);

    void undo();
    void redo();

    int id() const { return Id; }

private:
    QList<QGraphicsItem*> items;
    ItemGroup *g;
    Scene *s;
};

#endif //CROCHETCHARTCOMMANDS_H
