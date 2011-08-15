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
    
    SetCellStitch(Scene *s, CrochetCell *cell, QString newSt, QUndoCommand* parent = 0);

    void undo();
    void redo();

    int id() const { return Id; }
    
private:
    QString oldStitch;
    QString newStitch;
    CrochetCell *c;
    Scene *scene;
};

class SetCellColor : public QUndoCommand
{
public:
    enum { Id = 1110 };
    
    SetCellColor(Scene *s, CrochetCell *cell, QColor newCl, QUndoCommand* parent = 0);
    
    void undo();
    void redo();

    int id() const { return Id; }
    
private:
    QColor oldColor;
    QColor newColor;
    CrochetCell *c;
    Scene *scene;
};

class SetCellRotation : public QUndoCommand
{
public:
    enum { Id = 1120 };
    
    SetCellRotation(Scene *s, CrochetCell *cell, qreal baseRot, qreal diff, QUndoCommand* parent = 0);
    
    void undo();
    void redo();

    bool mergeWith(const QUndoCommand *command);
    
    int id() const { return Id; }
    
private:
    qreal baseRotation;
    CrochetCell *c;
    qreal delta;
    
    Scene *scene;
};

class SetItemCoordinates : public QUndoCommand
{
public:
    enum { Id = 1130 };

    SetItemCoordinates(Scene *s, QGraphicsItem *item, QPointF oldPos, QPointF newPos, QUndoCommand* parent = 0);

    void undo();
    void redo();

    bool mergeWith(const QUndoCommand *command);

    int id() const { return Id; }

private:
    QPointF oldCoord;
    QPointF newCoord;
    QGraphicsItem *i;

    Scene *scene;
};

class SetCellScale : public QUndoCommand
{
public:
    enum { Id = 1140 };

    SetCellScale(Scene *s, CrochetCell *cell, qreal scl, QUndoCommand* parent = 0);
    
    void undo();
    void redo();
    
    bool mergeWith(const QUndoCommand *command);
    
    int id() const { return Id; }
    
private:
    qreal oldScale;
    qreal newScale;
    
    CrochetCell *c;
    Scene *scene;
    
};

class AddCell : public QUndoCommand
{
public:
    enum { Id = 1150 };

    AddCell(Scene *s, QPoint pos, QUndoCommand* parent = 0);

    void redo();
    void undo();

    int id() const { return Id; }

    CrochetCell* cell() { return c; }
    
private:
    CrochetCell *c;
    QPoint position;

    Scene *scene;
};

class RemoveCell : public QUndoCommand
{
public:
    enum { Id = 1160 };

    RemoveCell(Scene *s, CrochetCell *c, QUndoCommand* parent = 0);

    void redo();
    void undo();

    int id() const { return Id; }

private:
    CrochetCell *c;
    QPoint position;

    Scene *scene;
};

class AddRow : public QUndoCommand
{
public:
    enum { Id = 1170 };

    AddRow(Scene *s, QPointF pos, QUndoCommand* parent = 0);

    void redo();
    void undo();

    int id() const { return Id; }

private:
    QPointF position;

    Scene *scene;
};

class RemoveRow : public QUndoCommand
{
public:
    enum { Id = 1180 };

    RemoveRow(Scene *s, QPointF pos, QUndoCommand* parent = 0);

    void redo();
    void undo();

    int id() const { return Id; }

private:
    QPointF position;

    Scene *scene;
};
#endif //CROCHETCHARTCOMMANDS_H
