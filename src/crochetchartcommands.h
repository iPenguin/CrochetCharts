/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef CROCHETCHARTCOMMANDS_H
#define CROCHETCHARTCOMMANDS_H

#include <QUndoCommand>

#include "cell.h"
#include "crochetscene.h"

class SetCellStitch : public QUndoCommand
{
public:
    enum { Id = 1100 };
    
    SetCellStitch(CrochetScene *s, CrochetCell *cell, QString newSt, QUndoCommand* parent = 0);

    void undo();
    void redo();

    int id() const { return Id; }
    
private:
    QString oldStitch;
    QString newStitch;
    CrochetCell *c;
    CrochetScene *scene;
};

class SetCellColor : public QUndoCommand
{
public:
    enum { Id = 1110 };
    
    SetCellColor(CrochetScene *s, CrochetCell *cell, QColor newCl, QUndoCommand* parent = 0);
    
    void undo();
    void redo();

    int id() const { return Id; }
    
private:
    QColor oldColor;
    QColor newColor;
    CrochetCell *c;
    CrochetScene *scene;
};

class SetCellRotation : public QUndoCommand
{
public:
    enum { Id = 1120 };
    
    SetCellRotation(CrochetScene *s, CrochetCell *cell, qreal baseRot, qreal diff, QUndoCommand* parent = 0);
    
    void undo();
    void redo();

    bool mergeWith(const QUndoCommand *command);
    
    int id() const { return Id; }
    
private:
    qreal baseRotation;
    CrochetCell *c;
    qreal delta;
    
    CrochetScene *scene;
};

class SetCellCoordinates : public QUndoCommand
{
public:
    enum { Id = 1130 };

    SetCellCoordinates(CrochetScene *s, CrochetCell *cell, QPointF oldPos, QPointF newPos, QUndoCommand* parent = 0);

    void undo();
    void redo();

    bool mergeWith(const QUndoCommand *command);

    int id() const { return Id; }

private:
    QPointF oldCoord;
    QPointF newCoord;
    CrochetCell *c;

    CrochetScene *scene;
};

class SetCellScale : public QUndoCommand
{
public:
    enum { Id = 1140 };

    SetCellScale(CrochetScene *s, CrochetCell *cell, qreal scl, QUndoCommand* parent = 0);
    
    void undo();
    void redo();
    
    bool mergeWith(const QUndoCommand *command);
    
    int id() const { return Id; }
    
private:
    qreal oldScale;
    qreal newScale;
    
    CrochetCell *c;
    CrochetScene *scene;
    
};

class AddCell : public QUndoCommand
{
public:
    enum { Id = 1150 };

    AddCell(CrochetScene *s, QPoint pos, QUndoCommand* parent = 0);

    void redo();
    void undo();

    int id() const { return Id; }

    CrochetCell* cell() { return c; }
    
private:
    CrochetCell *c;
    QPoint position;

    CrochetScene *scene;
};

class RemoveCell : public QUndoCommand
{
public:
    enum { Id = 1160 };

    RemoveCell(CrochetScene *s, CrochetCell *c, QUndoCommand* parent = 0);

    void redo();
    void undo();

    int id() const { return Id; }

private:
    CrochetCell *c;
    QPoint position;

    CrochetScene *scene;
};

class AddRow : public QUndoCommand
{
public:
    enum { Id = 1170 };

    AddRow(CrochetScene *s, QPointF pos, QUndoCommand* parent = 0);

    void redo();
    void undo();

    int id() const { return Id; }

private:
    QPointF position;

    CrochetScene *scene;
};

class RemoveRow : public QUndoCommand
{
public:
    enum { Id = 1180 };

    RemoveRow(CrochetScene *s, QPointF pos, QUndoCommand* parent = 0);

    void redo();
    void undo();

    int id() const { return Id; }

private:
    QPointF position;

    CrochetScene *scene;
};
#endif //CROCHETCHARTCOMMANDS_H
