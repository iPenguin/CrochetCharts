/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "scenerows.h"

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
#include "indicatorundo.h"

#include <QKeyEvent>
#include "stitchlibrary.h"

SceneRows::SceneRows(QObject *parent)
    : Scene(parent)
{
}

SceneRows::~SceneRows()
{

}

CrochetCell* SceneRows::cell(int row, int column)
{
    Q_ASSERT(mGrid.count() > row);
    if(mGrid[row].count() <= column)
        return 0;

    return mGrid[row][column];
}

CrochetCell* SceneRows::cell(QPoint position)
{
    return cell(position.y(), position.x());
}

void SceneRows::removeCell(CrochetCell *c)
{
    int y = findGridPosition(c).y();
    removeItem(c);
    for(int i = 0; i < mGrid.count(); ++i) {
        if (mGrid[i].contains(c)) {
            mGrid[i].removeOne(c);
        }
    }

    redistributeCells(y);
}

int SceneRows::rowCount()
{
    return mGrid.count();
}

int SceneRows::columnCount(int row)
{
    if(mGrid.count() <= row)
        return 0;
    return mGrid[row].count();
}

void SceneRows::appendCell(int row, CrochetCell *c, bool fromSave)
{
    Q_UNUSED(fromSave);
    //append any missing rows.
    if(mGrid.count() <= row) {
        for(int i = mGrid.count(); i < row + 1; ++i) {
            QList<CrochetCell*> row;
            mGrid.append(row);
        }
    }
    
    addCell(QPoint(mGrid[row].count(), row), c);

    int col = mGrid[row].count() -1;
    setCellPosition(row, col, c, mGrid[row].count());
    c->setColor(QColor(Qt::white));

}

void SceneRows::addCell(QPoint p, CrochetCell* c)
{

    //TODO: simplify the connect() statements...
    addItem(c);
    int x = p.x();

    if(mGrid.count() <= p.y()) {
        QList<CrochetCell*> row;
        mGrid.append(row);
    }

    if(mGrid[p.y()].count() <= p.x())
            x = mGrid[p.y()].count();

    mGrid[p.y()].insert(x, c);

    setCellPosition(p.y(), x, c, mGrid[p.y()].count());

    connect(c, SIGNAL(stitchChanged(QString,QString)), this, SIGNAL(stitchChanged(QString,QString)));
    connect(c, SIGNAL(colorChanged(QString,QString)), this, SIGNAL(colorChanged(QString,QString)));
    connect(c, SIGNAL(stitchChanged(QString,QString)), this, SLOT(stitchUpdated(QString,QString)));

    redistributeCells(p.y());
}

void SceneRows::setCellPosition(int row, int column, CrochetCell *c, int columns, bool updateAnchor)
{
    Q_UNUSED(columns);
    
    c->setPos(column*mDefaultSize.width() + column*5, row*mDefaultSize.height());
    if(updateAnchor || c->anchor().isNull())
        c->setAnchor(column*mDefaultSize.width() + column*5, row*mDefaultSize.height());
    c->setColor(QColor(Qt::white));

    //FIXME: set tooltips from bottom right to top left.
    c->setToolTip(tr("Row: %1, St: %2").arg(row+1).arg(column+1));
}

void SceneRows::redistributeCells(int row)
{
    if(row >= mGrid.count())
        return;
    int columns = mGrid[row].count();

    for(int i = 0; i < columns; ++i) {
        CrochetCell *c = mGrid[row].at(i);
        setCellPosition(row, i, c, columns, true);
    }
}

void SceneRows::createChart(int rows, int cols, QString stitch, QSizeF rowSize)
{
    mDefaultSize = rowSize;

    for(int i = 0; i < rows; ++i)
        createRow(i, cols, stitch);

    initDemoBackground();
}

void SceneRows::createRow(int row, int columns, QString stitch)
{
    CrochetCell *c = 0;
    
    QList<CrochetCell*> modelRow;
    for(int i = 0; i < columns; ++i) {
        c = new CrochetCell();
        connect(c, SIGNAL(stitchChanged(QString,QString)), this, SIGNAL(stitchChanged(QString,QString)));
        connect(c, SIGNAL(colorChanged(QString,QString)), this, SIGNAL(colorChanged(QString,QString)));
        connect(c, SIGNAL(stitchChanged(QString,QString)), this, SLOT(stitchUpdated(QString,QString)));
        
        c->setStitch(stitch, (row % 2));
        addItem(c);
        modelRow.append(c);
        c->setColor(QColor(Qt::white));
        setCellPosition(row, i, c, columns);
    }
    mGrid.append(modelRow);

}

void SceneRows::stitchUpdated(QString oldSt, QString newSt)
{
    Q_UNUSED(newSt);
    
    if(oldSt.isEmpty() || oldSt.isNull())
        return;
    
    CrochetCell *c = qobject_cast<CrochetCell*>(sender());
    if(!c)
        return;

    QPoint pos = findGridPosition(c);
    
}

QPoint SceneRows::findGridPosition(CrochetCell* c)
{
    for(int y = 0; y < mGrid.count(); ++y) {
        if(mGrid[y].contains(c)) {
            return QPoint(mGrid[y].indexOf(c), y);
        }
    }
    
    return QPoint();
}

qreal SceneRows::scenePosToAngle(QPointF pt)
{

    qreal rads = atan2(pt.x(), pt.y());
    qreal angleX = rads * 180 / M_PI;
    
    return -angleX;
}

void SceneRows::keyReleaseEvent(QKeyEvent* keyEvent)
{
    Scene::keyReleaseEvent(keyEvent);
}

void SceneRows::mousePressEvent(QGraphicsSceneMouseEvent *e)
{
    Scene::mousePressEvent(e);
    
    switch(mMode) {
        case SceneRows::AngleMode:
            angleModeMousePress(e);
            break;
        default:
            break;
    }

}

void SceneRows::mouseMoveEvent(QGraphicsSceneMouseEvent *e)
{

    switch(mMode) {
        case SceneRows::StitchMode:
            stitchModeMouseMove(e);
            break;
        case SceneRows::ColorMode:
            colorModeMouseMove(e);
            break;
        case SceneRows::AngleMode:
            angleModeMouseMove(e);
            return;
        case SceneRows::StretchMode:
            stretchModeMouseMove(e);
            break;
        case SceneRows::IndicatorMode:
            indicatorModeMouseMove(e);
            break;
        default:
            break;
    }

    Scene::mouseMoveEvent(e);
    
}

void SceneRows::mouseReleaseEvent(QGraphicsSceneMouseEvent *e)
{
    
    switch(mMode) {
        case SceneRows::StitchMode:
            stitchModeMouseRelease(e);
            break;
        case SceneRows::ColorMode:
            colorModeMouseRelease(e);
            break;
        case SceneRows::AngleMode:
            angleModeMouseRelease(e);
            break;
        case SceneRows::IndicatorMode:
            indicatorModeMouseRelease(e);
            break;
        default:
            break;
    }

    Scene::mouseReleaseEvent(e);
}

void SceneRows::colorModeMouseMove(QGraphicsSceneMouseEvent* e)
{
    if(e->buttons() != Qt::LeftButton)
        return;
    
    if(!mCurCell)
        return;
    
    if(mCurCell->color() != mEditBgColor)
        mUndoStack.push(new SetCellColor(this, mCurCell, mEditBgColor));
}

void SceneRows::colorModeMouseRelease(QGraphicsSceneMouseEvent* e)
{
    Q_UNUSED(e);
    if(!mCurCell)
        return;

    if(mCurCell->color() != mEditBgColor)
        mUndoStack.push(new SetCellColor(this, mCurCell, mEditBgColor));
}

void SceneRows::stitchModeMouseMove(QGraphicsSceneMouseEvent* e)
{
    if(e->buttons() != Qt::LeftButton)
        return;
    
    if(!mCurCell)
        return;
    
    //FIXME: if not dragging objects around allow stitch painting.
}

void SceneRows::stitchModeMouseRelease(QGraphicsSceneMouseEvent* e)
{
    //FIXME: foreach(stitch in selection()) create an undo group event.
    if(mCurCell) {
        
    if(mCurCell->name() != mEditStitch && !mMoving)
        mUndoStack.push(new SetCellStitch(this, mCurCell, mEditStitch));
    
        mCurCell = 0;
    } else if(!mRubberBand && !mMoving){
        int x = ceil(e->scenePos().x() / mDefaultSize.width()) - 1;
        int y = ceil(e->scenePos().y() / mDefaultSize.height()) - 1;

        if(e->button() == Qt::LeftButton && !(e->modifiers() & Qt::ControlModifier)) {

            AddCell *addCell = new AddCell(this, QPoint(x, y));
            CrochetCell *c = addCell->cell();
            c->setStitch(mEditStitch, (y % 2));
            undoStack()->push(addCell);

        } else {
            if(!mCurCell)
                return;

            undoStack()->push(new RemoveCell(this, mCurCell));
            mCurCell = 0;
        }
    }
}

void SceneRows::angleModeMousePress(QGraphicsSceneMouseEvent *e)
{
    if(!mCurCell)
        return;
    
    qreal value = acos(mCurCell->transform().m11()) / M_PI * 180;
    if(e->scenePos().x() < 0 && e->scenePos().y() >= 0)
        mCurCellRotation = 180 - value;
    else if(e->scenePos().x() < 0 && e->scenePos().y() < 0)
        mCurCellRotation = 180 - value;
    else
        mCurCellRotation = value;
   
}

void SceneRows::angleModeMouseMove(QGraphicsSceneMouseEvent *e)
{
    if(!mCurCell)
        return;

    qreal pvtPt = mCurCell->stitch()->width()/2;
    qDebug() << pvtPt;
    QPointF origin = mCurCell->mapToScene(pvtPt, 0);
    QPointF first = e->buttonDownScenePos(Qt::LeftButton);
    QPointF second = e->scenePos();
    QPointF rel1 = QPointF(first.x() - origin.x(), first.y() - origin.y());
    QPointF rel2 = QPointF(second.x() - origin.x(), second.y() - origin.y());
    qreal angle1 = scenePosToAngle(rel1);
    qreal angle2 = scenePosToAngle(rel2);

    mUndoStack.push(new SetCellRotation(this, mCurCell, mCurCellRotation, (angle1 - angle2)));

}

void SceneRows::angleModeMouseRelease(QGraphicsSceneMouseEvent *e)
{
    Q_UNUSED(e);
    mCurCellRotation = 0;
}

void SceneRows::stretchModeMouseMove(QGraphicsSceneMouseEvent* e)
{
    if(!mCurCell)
        return;
    
    QPointF cur = e->scenePos();
    
    qreal scale;
    qreal diff = (mLeftButtonDownPos.y() - cur.y());

    scale = -diff/mCurCell->boundingRect().height();

    qDebug() << mLeftButtonDownPos << cur << diff << scale;
    mUndoStack.push(new SetCellScale(this, mCurCell, scale));
}

void SceneRows::indicatorModeMouseMove(QGraphicsSceneMouseEvent *e)
{
    if(e->buttons() != Qt::LeftButton)
        return;

    mMoving = true;

    if(!mCurIndicator)
        return;

    QPointF start = e->buttonDownScenePos(Qt::LeftButton);
    
    QPointF delta =  QPointF(e->scenePos().x() - start.x(), e->scenePos().y() - start.y());
    QPointF newPos = QPointF(mCellStartPos.x() + delta.x(), mCellStartPos.y() + delta.y());
    
    undoStack()->push(new MoveIndicator(this, mCurIndicator, newPos));

}

void SceneRows::indicatorModeMouseRelease(QGraphicsSceneMouseEvent *e)
{
    //if right click or ctrl-click remove the indicator.
    if(e->button() == Qt::RightButton || (e->button() == Qt::LeftButton && e->modifiers() == Qt::ControlModifier)) {
        if(mCurIndicator) {
            undoStack()->push(new RemoveIndicator(this, mCurIndicator));
        }
        return;
    }

    //if we're moving another indicator we shouldn't be creating a new one.
    if(mMoving) {
        mMoving = false;
        return;
    }

    if(!mCurIndicator) {

        QPointF pt = e->buttonDownScenePos(Qt::LeftButton);
        //FIXME: dont hard code the offset for the indicator.
        pt = QPointF(pt.x() - 10, pt.y() - 10);

        undoStack()->push(new AddIndicator(this, pt));

        //connect(i, SIGNAL(lostFocus(Indicator*)), this, SLOT(editorLostFocus(Indicator*)));
        //connect(i, SIGNAL(selectedChange(QGraphicsItem*)), this, SIGNAL(itemSelected(QGraphicsItem*)));
    } else {
        mCurIndicator->setTextInteractionFlags(Qt::TextEditorInteraction);
    }
    
}
