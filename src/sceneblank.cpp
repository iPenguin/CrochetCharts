/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "sceneblank.h"

#include "crochetcell.h"

#include <QFontMetrics>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsSceneEvent>
#include <QApplication> //start drag min distance.

#include <math.h>

#include <QDebug>

#include "chartview.h"

#include "settings.h"
#include "stitchset.h"
#include "appinfo.h"
#include "crochetchartcommands.h"

#include <QKeyEvent>
#include "stitchlibrary.h"

SceneBlank::SceneBlank(QObject* parent)
    : Scene(parent)
{
}

SceneBlank::~SceneBlank()
{
}

CrochetCell* SceneBlank::cell(int row, int column)
{
    return new CrochetCell();
}

CrochetCell* SceneBlank::cell(QPoint position)
{
    return cell(position.y(), position.x());
}

void SceneBlank::createChart(int rows, int cols, QString stitch, QSizeF rowSize)
{
    Q_UNUSED(rows);
    Q_UNUSED(cols);
    Q_UNUSED(stitch);
    Q_UNUSED(rowSize);
    
    initDemoBackground();
}

void SceneBlank::createRow(int row, int columns, QString stitch)
{
    Q_UNUSED(row);
    Q_UNUSED(columns);
    Q_UNUSED(stitch);
}

void SceneBlank::stitchModeMouseMove(QGraphicsSceneMouseEvent* e)
{
    Q_UNUSED(e);
}

void SceneBlank::stitchModeMouseRelease(QGraphicsSceneMouseEvent* e)
{
    //FIXME: foreach(stitch in selection()) create an undo group event.
    if(mCurCell) {

    if(mCurCell->name() != mEditStitch && !mMoving)
        undoStack()->push(new SetCellStitch(this, mCurCell, mEditStitch));

        mCurCell = 0;
    } else if(!mRubberBand && !mMoving){

        if(e->button() == Qt::LeftButton && !(e->modifiers() & Qt::ControlModifier)) {

            AddCell* addCell = new AddCell(this, e->scenePos());
            undoStack()->push(addCell);
            addCell->cell()->setStitch(mEditStitch);

        } else {
            if(!mCurCell)
                return;

            undoStack()->push(new RemoveCell(this, mCurCell));
            mCurCell = 0;
        }
    }
}
