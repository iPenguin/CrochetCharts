/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef CROCHETCHARTCOMMANDS_H
#define CROCHETCHARTCOMMANDS_H

#include <QUndoCommand>

#include "cell.h"

class SetCellStitch : public QUndoCommand
{
public:
     explicit SetCellStitch(Cell *cell, QString newSt, QUndoCommand* parent = 0);

    void undo();
    void redo();
    
private:
    QString oldStitch;
    QString newStitch;
    Cell *c;
};


class SetCellColor : public QUndoCommand
{
public:
    explicit SetCellColor(Cell *cell, QColor newCl, QUndoCommand* parent = 0);
    
    void undo();
    void redo();
    
private:
    QColor oldColor;
    QColor newColor;
    Cell *c;
};

class SetCellRotation : public QUndoCommand
{
public:
    enum { Id = 1234 };
    
    SetCellRotation(Cell *cell, qreal rot, QUndoCommand* parent = 0);
    
    void undo();
    void redo();

    bool mergeWith(const QUndoCommand *command);
    
    int id() const { return Id; }
    
private:
    qreal rotation;
    Cell *c;
};

#endif //CROCHETCHARTCOMMANDS_H
