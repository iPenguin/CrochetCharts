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
     explicit SetCellStitch(CrochetScene *s, QPoint pos, QString newSt, QUndoCommand* parent = 0);

    void undo();
    void redo();
    
private:
    QString oldStitch;
    QString newStitch;
    QPoint position;
    CrochetScene *scene;
};


class SetCellColor : public QUndoCommand
{
public:
    explicit SetCellColor(CrochetScene *s, QPoint pos, QColor newCl, QUndoCommand* parent = 0);
    
    void undo();
    void redo();
    
private:
    QColor oldColor;
    QColor newColor;
    QPoint position;
    CrochetScene *scene;
};

class SetCellRotation : public QUndoCommand
{
public:
    enum { Id = 1234 };
    
    SetCellRotation(CrochetScene *s, QPoint pos, qreal baseRot, qreal diff, QUndoCommand* parent = 0);
    
    void undo();
    void redo();

    bool mergeWith(const QUndoCommand *command);
    
    int id() const { return Id; }
    
private:
    qreal baseRotation;
    QPoint position;
    qreal delta;
    
    CrochetScene *scene;
};

class SetCellCoordinates : public QUndoCommand
{
public:
    enum { Id = 1235 };

    SetCellCoordinates(CrochetScene *s, QPoint pos, QPointF oldPos, QPointF newPos, QUndoCommand* parent = 0);

    void undo();
    void redo();

    bool mergeWith(const QUndoCommand *command);

    int id() const { return Id; }

private:
    QPointF oldCoord;
    QPointF newCoord;
    QPoint position;

    CrochetScene *scene;
};

class SetCellScale : public QUndoCommand
{
public:
    enum { Id = 1236 };

    SetCellScale(CrochetScene *s, QPoint pos, qreal scl, QUndoCommand* parent = 0);
    
    void undo();
    void redo();
    
    bool mergeWith(const QUndoCommand *command);
    
    int id() const { return Id; }
    
private:
    qreal delta;
    QPoint position;
    QList<qreal> deltas;
    QList<qreal> baseSize;
    
    CrochetScene *scene;
    
};

class AddIndicator : public QUndoCommand
{
public:
    enum { Id = 1237 };

    AddIndicator(CrochetScene *s, QPointF pos, QUndoCommand* parent = 0);

    void undo();
    void redo();

    int id() const { return Id; }

private:
    
    QPointF position;
    
    CrochetScene *scene;
};

class RemoveIndicator : public QUndoCommand
{
public:
    enum { Id = 1238 };

    RemoveIndicator(CrochetScene *s, QPointF pos, QUndoCommand* parent = 0);

    void redo();
    void undo();

    int id() const { return Id; }

private:
    QPointF position;

    CrochetScene *scene;
};

class AddStitch : public QUndoCommand
{

};

class RemoveStitch : public QUndoCommand
{

};

#endif //CROCHETCHARTCOMMANDS_H
