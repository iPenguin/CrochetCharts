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
#include "indicatorundo.h"

#include <QKeyEvent>
#include "stitchlibrary.h"

SceneBlank::SceneBlank(QObject *parent)
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

void SceneBlank::removeCell(CrochetCell *c)
{
    removeItem(c);
}

int SceneBlank::rowCount()
{
    return 1;
}

int SceneBlank::columnCount(int row)
{
    return items().count();
}

void SceneBlank::appendCell(int row, CrochetCell *c, bool fromSave)
{
    Q_UNUSED(fromSave);
    //append any missing rows.
    addItem(c);
    
    c->setColor(QColor(Qt::white));
   
}

void SceneBlank::addCell(QPoint p, CrochetCell* c)
{

    //TODO: simplify the connect() statements...
    addItem(c);
qDebug() << "wrong add item";
    connect(c, SIGNAL(stitchChanged(QString,QString)), this, SIGNAL(stitchChanged(QString,QString)));
    connect(c, SIGNAL(colorChanged(QString,QString)), this, SIGNAL(colorChanged(QString,QString)));

}

void SceneBlank::createChart(int rows, int cols, QString stitch, QSizeF rowSize)
{

    initDemoBackground();
}

void SceneBlank::createRow(int row, int columns, QString stitch)
{
    Q_UNUSED(row);
    Q_UNUSED(columns);
    Q_UNUSED(stitch);
}

QPoint SceneBlank::findGridPosition(CrochetCell* c)
{
    return QPoint();
}

void SceneBlank::setCellPosition(int row, int column, CrochetCell* c, int columns, bool updateAnchor)
{
    Q_UNUSED(row);
    Q_UNUSED(column);
    Q_UNUSED(c);
    Q_UNUSED(columns);
    Q_UNUSED(updateAnchor);
    //c->setPos(row, column);
}

void SceneBlank::keyReleaseEvent(QKeyEvent* keyEvent)
{
    Scene::keyReleaseEvent(keyEvent);
}

void SceneBlank::mousePressEvent(QGraphicsSceneMouseEvent *e)
{
    Scene::mousePressEvent(e);
    
    switch(mMode) {
        default:
            break;
    }

    Scene::mousePressEvent(e);
}

void SceneBlank::mouseMoveEvent(QGraphicsSceneMouseEvent *e)
{

    switch(mMode) {
        case SceneBlank::StitchMode:
            stitchModeMouseMove(e);
            break;
        default:
            break;
    }

    Scene::mouseMoveEvent(e);
}

void SceneBlank::mouseReleaseEvent(QGraphicsSceneMouseEvent *e)
{
 
    switch(mMode) {
        case SceneBlank::StitchMode:
            stitchModeMouseRelease(e);
            break;
        default:
            break;
    }

    Scene::mouseReleaseEvent(e);
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
        mUndoStack.push(new SetCellStitch(this, mCurCell, mEditStitch));

        mCurCell = 0;
    } else if(!mRubberBand && !mMoving){

        if(e->button() == Qt::LeftButton && !(e->modifiers() & Qt::ControlModifier)) {

            CrochetCell *c = new CrochetCell();
            addItem(c);
            c->setStitch(mEditStitch, false);
            c->setPos(e->scenePos());
            qDebug() << "set pos" << e->scenePos();
        } else {
            if(!mCurCell)
                return;

            undoStack()->push(new RemoveCell(this, mCurCell));
            mCurCell = 0;
        }
    }
}
