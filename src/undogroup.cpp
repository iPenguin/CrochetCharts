/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "undogroup.h"
#include <QUndoStack>

#include <QDebug>

UndoGroup::UndoGroup(QObject *parent)
    : QUndoGroup(parent)
{
    connect(this, SIGNAL(cleanChanged(bool)), this, SLOT(checkAllCleanStates()));
    
}

void UndoGroup::addStack(QUndoStack *stack)
{

    connect(stack, SIGNAL(canUndoChanged(bool)), SLOT(checkAllCleanStates()));
    QUndoGroup::addStack(stack);
}

void UndoGroup::checkAllCleanStates()
{
    bool clean = true;
    
    foreach(QUndoStack *stack, stacks()) {
        if(!stack->isClean()) {
            clean = false;
            break;
        }
    }

    //TODO: don't emit on every entry only when the state changes.
    emit isModified(!clean);
}
