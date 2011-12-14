/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef CROCHETCHARTCOMMANDS_H
#define CROCHETCHARTCOMMANDS_H

#include <QUndoCommand>

#include "cell.h"
#include "scene.h"
#include "item.h"

class SetCellStitch : public QUndoCommand
{
public:
    enum { Id = 1100 };
    
    SetCellStitch(Scene* s, Cell* cell, QString newSt, QUndoCommand* parent = 0);

    void undo();
    void redo();

    int id() const { return Id; }
    
private:
    QString oldStitch;
    QString newStitch;
    Cell* c;
    Scene* scene;
};

class SetCellColor : public QUndoCommand
{
public:
    enum { Id = 1110 };
    
    SetCellColor(Scene* s, Cell* cell, QColor newCl, QUndoCommand* parent = 0);
    
    void undo();
    void redo();

    int id() const { return Id; }
    
private:
    QColor oldColor;
    QColor newColor;
    Cell* c;
    Scene* scene;
};

class SetItemRotation : public QUndoCommand
{
public:
    enum { Id = 1120 };
    
    SetItemRotation(Scene* s, QGraphicsItem* item, qreal oldAngl, QPointF pivotPt, QUndoCommand* parent = 0);
    
    void undo();
    void redo();
    
    int id() const { return Id; }
    
private:
    QGraphicsItem* i;
    qreal oldAngle;
    qreal newAngle;
    QPointF pvtPt;
    
    Scene* scene;
};

class SetItemsRotation : public QUndoCommand
{
public:
    enum { Id = 1125 };

    SetItemsRotation(Scene* s, QList<QGraphicsItem*> itms, qreal degrees, QUndoCommand* parent = 0);

    void undo();
    void redo();

    int id() const { return Id; }

private:
    QList<QGraphicsItem*> items;
    qreal oldAngle;
    qreal newAngle;

    QPointF pivotPoint;
    Scene* scene;
};

class SetItemCoordinates : public QUndoCommand
{
public:
    enum { Id = 1130 };

    SetItemCoordinates(Scene* s, QGraphicsItem* item, QPointF oldPos, QUndoCommand* parent = 0);

    void undo();
    void redo();

    int id() const { return Id; }

private:
    QPointF oldCoord;
    QPointF newCoord;
    QGraphicsItem* i;

    Scene* scene;
};

class SetItemScale : public QUndoCommand
{
public:
    enum { Id = 1140 };

    SetItemScale(Scene* s, Item* item, QPointF oldScle, QPointF pvtPt, QUndoCommand* parent = 0);
    
    void undo();
    void redo();
    
    int id() const { return Id; }
    
private:
    QPointF oldScale;
    QPointF newScale;

    QPointF pivotPt;
    
    Item* i;
    Scene* scene;
    
};

class AddCell : public QUndoCommand
{
public:
    enum { Id = 1150 };

    AddCell(Scene* s, QPointF pos, QUndoCommand* parent = 0);

    void redo();
    void undo();

    int id() const { return Id; }

    Cell* cell() { return c; }
    
private:
    Cell* c;
    QPointF position;

    Scene* scene;
};

class RemoveCell : public QUndoCommand
{
public:
    enum { Id = 1160 };

    RemoveCell(Scene* s, Cell* c, QUndoCommand* parent = 0);

    void redo();
    void undo();

    int id() const { return Id; }

private:
    Cell* c;
    QPointF position;

    Scene* scene;
};

class GroupItems : public QUndoCommand
{
public:
    enum { Id = 1200 };

    GroupItems(Scene* s, QList<QGraphicsItem*> itemList, QUndoCommand* parent = 0);

    void undo();
    void redo();

    int id() const { return Id; }

    ItemGroup* group() { return mGroup; }

private:
    QList<QGraphicsItem*> items;
    ItemGroup* mGroup;
    Scene* scene;
};

class UngroupItems : public QUndoCommand
{
public:
    enum { Id = 1210 };

    UngroupItems(Scene* s, ItemGroup* grp, QUndoCommand* parent = 0);

    void undo();
    void redo();

    int id() const { return Id; }

private:
    QList<QGraphicsItem*> items;
    ItemGroup* group;
    Scene* scene;
};

class RemoveGroup : public QUndoCommand
{
public:
    enum { Id = 1220 };

    RemoveGroup(Scene* s, ItemGroup* grp, QUndoCommand* parent = 0);

    void undo();
    void redo();

    int id() const { return Id; }

private:
    QList<QGraphicsItem*> items;
    ItemGroup* group;
    Scene* scene;
};

#endif //CROCHETCHARTCOMMANDS_H
