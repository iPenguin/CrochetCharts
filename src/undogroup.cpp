/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "undogroup.h"
#include <QUndoStack>

UndoGroup::UndoGroup(QObject *parent)
    : QUndoGroup(parent), mClean(true)
{
    connect(this, SIGNAL(cleanChanged(bool)), this, SLOT(checkAllCleanStates()));
    
}

void UndoGroup::checkAllCleanStates()
{
    bool clean = true;
    
    foreach(QUndoStack *stack, stacks()) {
        if(!stack->isClean())
            clean = false;
    }

    if(mClean != clean) {
        mClean = clean;
        emit documentCleanChanged(clean);
    }
}
