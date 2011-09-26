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
    
    SetCellStitch(Scene* s, CrochetCell* cell, QString newSt, QUndoCommand* parent = 0);

    void undo();
    void redo();

    int id() const { return Id; }
    
private:
    QString oldStitch;
    QString newStitch;
    CrochetCell* c;
    Scene* scene;
};

class SetCellColor : public QUndoCommand
{
public:
    enum { Id = 1110 };
    
    SetCellColor(Scene* s, CrochetCell* cell, QColor newCl, QUndoCommand* parent = 0);
    
    void undo();
    void redo();

    int id() const { return Id; }
    
private:
    QColor oldColor;
    QColor newColor;
    CrochetCell* c;
    Scene* scene;
};

class SetCellRotation : public QUndoCommand
{
public:
    enum { Id = 1120 };
    
    SetCellRotation(Scene* s, CrochetCell* cell, qreal oldAngl, QPointF pivotPt, QUndoCommand* parent = 0);
    
    void undo();
    void redo();
    
    int id() const { return Id; }
    
private:
    CrochetCell* c;
    qreal oldAngle;
    qreal newAngle;
    QPointF pvtPt;
    
    Scene* scene;
};

class SetItemRotation : public QUndoCommand
{
public:
    enum { Id = 1125 };

    SetItemRotation(Scene* s, QList<QGraphicsItem*> itms, qreal degrees, QUndoCommand* parent = 0);

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

class SetCellScale : public QUndoCommand
{
public:
    enum { Id = 1140 };

    SetCellScale(Scene* s, CrochetCell* cell, QPointF oldScle, QPointF pvtPt, QUndoCommand* parent = 0);
    
    void undo();
    void redo();
    
    int id() const { return Id; }
    
private:
    QPointF oldScale;
    QPointF newScale;

    QPointF pivotPt;
    
    CrochetCell* c;
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

    CrochetCell* cell() { return c; }
    
private:
    CrochetCell* c;
    QPointF position;

    Scene* scene;
};

class RemoveCell : public QUndoCommand
{
public:
    enum { Id = 1160 };

    RemoveCell(Scene* s, CrochetCell* c, QUndoCommand* parent = 0);

    void redo();
    void undo();

    int id() const { return Id; }

private:
    CrochetCell* c;
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

    QGraphicsItemGroup* group() { return mGroup; }

private:
    QList<QGraphicsItem*> items;
    QGraphicsItemGroup* mGroup;
    Scene* scene;
};

class UngroupItems : public QUndoCommand
{
public:
    enum { Id = 1210 };

    UngroupItems(Scene* s, QGraphicsItemGroup* grp, QUndoCommand* parent = 0);

    void undo();
    void redo();

    int id() const { return Id; }

private:
    QList<QGraphicsItem*> items;
    QGraphicsItemGroup* group;
    Scene* scene;
};

class RemoveGroup : public QUndoCommand
{
public:
    enum { Id = 1220 };

    RemoveGroup(Scene* s, QGraphicsItemGroup* grp, QUndoCommand* parent = 0);

    void undo();
    void redo();

    int id() const { return Id; }

private:
    QList<QGraphicsItem*> items;
    QGraphicsItemGroup* group;
    Scene* scene;
};

#endif //CROCHETCHARTCOMMANDS_H
