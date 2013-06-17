/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef CROCHETCHARTCOMMANDS_H
#define CROCHETCHARTCOMMANDS_H

#include <QUndoCommand>

#include "cell.h"
#include "scene.h"

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
