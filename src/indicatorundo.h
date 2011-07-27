/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef INDICATORUNDO_H
#define INDICATORUNDO_H

#include <QUndoCommand>

class Indicator;
class CrochetScene;

#include <QPointF>
#include <QString>
#include <QColor>

class AddIndicator : public QUndoCommand
{
public:
    enum { Id = 2200 };

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
    enum { Id = 2210 };

    RemoveIndicator(CrochetScene *s, Indicator *i, QUndoCommand* parent = 0);

    void redo();
    void undo();

    int id() const { return Id; }

private:
    QPointF position;
    QString text;

    QColor bgColor;
    QColor textColor;

    CrochetScene *scene;
};

class MoveIndicator : public QUndoCommand
{
public:
    enum { Id = 2220 };

    MoveIndicator(CrochetScene *s, QPointF origPos, QPointF newPos, QUndoCommand* parent = 0);

    void redo();
    void undo();

    bool mergeWith(const QUndoCommand *command);
    
    int id() const { return Id; }

private:
    QPointF origPosition;
    QPointF newPosition;

    CrochetScene *scene;
};

class ChangeTextIndicator : public QUndoCommand
{
public:
    enum { Id = 2230 };

    ChangeTextIndicator(CrochetScene *s, QPointF pos, QString text, QUndoCommand *parent = 0);

    void redo();
    void undo();

    int id() const { return Id; }

private:
    QString newText;
    QString origText;

    QPointF position;

    CrochetScene *scene;
    
};

#endif //INDICATORUNDO_H