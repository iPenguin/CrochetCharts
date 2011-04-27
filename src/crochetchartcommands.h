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
    
    SetCellRotation(CrochetScene *s, QPoint pos, QPointF fst, QPointF snd, qreal baseRot, QUndoCommand* parent = 0);
    
    void undo();
    void redo();

    bool mergeWith(const QUndoCommand *command);
    
    int id() const { return Id; }
    
private:
    qreal baseRotation;
    QPoint position;
    QPointF first;
    QPointF second;
    
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

#endif //CROCHETCHARTCOMMANDS_H
