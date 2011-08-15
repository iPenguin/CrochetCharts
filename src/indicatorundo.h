/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef INDICATORUNDO_H
#define INDICATORUNDO_H

#include <QUndoCommand>

class Indicator;
class Scene;

#include <QPointF>
#include <QString>
#include <QColor>

class AddIndicator : public QUndoCommand
{
public:
    enum { Id = 2200 };

    AddIndicator(Scene *s, QPointF pos, QUndoCommand* parent = 0);

    void undo();
    void redo();

    int id() const { return Id; }

private:
    QPointF position;

    Indicator *item;
    Scene *scene;
};

class RemoveIndicator : public QUndoCommand
{
public:
    enum { Id = 2210 };

    RemoveIndicator(Scene *s, Indicator *i, QUndoCommand* parent = 0);

    void redo();
    void undo();

    int id() const { return Id; }

private:
    QPointF position;
    Indicator *item;

    Scene *scene;
};

class MoveIndicator : public QUndoCommand
{
public:
    enum { Id = 2220 };

    MoveIndicator(Scene *s, Indicator *item, QPointF newPos, QUndoCommand* parent = 0);

    void redo();
    void undo();

    bool mergeWith(const QUndoCommand *command);
    
    int id() const { return Id; }

private:
    Indicator *i;
    QPointF origPosition;
    QPointF newPosition;

    Scene *scene;
};

class ChangeTextIndicator : public QUndoCommand
{
public:
    enum { Id = 2230 };

    ChangeTextIndicator(Scene *s, Indicator *item, QString text, QUndoCommand *parent = 0);

    void redo();
    void undo();

    int id() const { return Id; }

private:
    Indicator *i;
    QString newText;
    QString origText;

    Scene *scene;
};

#endif //INDICATORUNDO_H
