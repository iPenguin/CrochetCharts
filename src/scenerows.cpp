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

void SceneRows::removeCell(CrochetCell *c)
{
    int y = findGridPosition(c).y();
    removeItem(c);
    for(int i = 0; i < grid().count(); ++i) {
        if (grid()[i].contains(c)) {
            grid()[i].removeOne(c);
        }
    }

    redistributeCells(y);
}

int SceneRows::rowCount()
{
    return grid().count();
}

int SceneRows::columnCount(int row)
{
    if(grid().count() <= row)
        return 0;
    return grid()[row].count();
}

void SceneRows::appendCell(int row, CrochetCell *c, bool fromSave)
{
    Q_UNUSED(fromSave);
    //append any missing rows.
    if(grid().count() <= row) {
        for(int i = grid().count(); i < row + 1; ++i) {
            QList<CrochetCell*> row;
            grid().append(row);
        }
    }
    /* QPoint(grid()[row].count(), row)*/
    addCell(c, QPointF());

    int col = grid()[row].count() -1;
    setCellPosition(row, col, c, grid()[row].count());
    c->setColor(QColor(Qt::white));

}

void SceneRows::addCell(CrochetCell* c, QPointF p)
{

    //TODO: simplify the connect() statements...
    addItem(c);
    int x = p.x();

    if(grid().count() <= p.y()) {
        QList<CrochetCell*> row;
        grid().append(row);
    }

    if(grid()[p.y()].count() <= p.x())
            x = grid()[p.y()].count();

    grid()[p.y()].insert(x, c);

    setCellPosition(p.y(), x, c, grid()[p.y()].count());

    connect(c, SIGNAL(stitchChanged(QString,QString)), this, SIGNAL(stitchChanged(QString,QString)));
    connect(c, SIGNAL(colorChanged(QString,QString)), this, SIGNAL(colorChanged(QString,QString)));

    redistributeCells(p.y());
}

void SceneRows::setCellPosition(int row, int column, CrochetCell *c, int columns, bool updateAnchor)
{
    Q_UNUSED(columns);
    
    c->setPos(column*defaultSize().width() + column*5, row*defaultSize().height());
    if(updateAnchor || c->anchor().isNull())
        c->setAnchor(column*defaultSize().width() + column*5, row*defaultSize().height());
    c->setColor(QColor(Qt::white));

    //FIXME: set tooltips from bottom right to top left.
    c->setToolTip(tr("Row: %1, St: %2").arg(row+1).arg(column+1));
}

void SceneRows::redistributeCells(int row)
{
    if(row >= grid().count())
        return;
    int columns = grid()[row].count();

    for(int i = 0; i < columns; ++i) {
        CrochetCell *c = grid()[row].at(i);
        setCellPosition(row, i, c, columns, true);
    }
}

void SceneRows::createChart(int rows, int cols, QString stitch, QSizeF rowSize)
{
    defaultSize() = rowSize;

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
        
        c->setStitch(stitch, (row % 2));
        addItem(c);
        modelRow.append(c);
        c->setColor(QColor(Qt::white));
        setCellPosition(row, i, c, columns);
    }
    grid().append(modelRow);

}

QPoint SceneRows::findGridPosition(CrochetCell* c)
{
    for(int y = 0; y < grid().count(); ++y) {
        if(grid()[y].contains(c)) {
            return QPoint(grid()[y].indexOf(c), y);
        }
    }
    
    return QPoint();
}

void SceneRows::keyReleaseEvent(QKeyEvent* keyEvent)
{
    Scene::keyReleaseEvent(keyEvent);
}

void SceneRows::mousePressEvent(QGraphicsSceneMouseEvent *e)
{
    Scene::mousePressEvent(e);
}

void SceneRows::mouseMoveEvent(QGraphicsSceneMouseEvent *e)
{

    switch(mMode) {
        case Scene::StitchMode:
            stitchModeMouseMove(e);
            break;
        default:
            break;
    }

    Scene::mouseMoveEvent(e);
    
}

void SceneRows::mouseReleaseEvent(QGraphicsSceneMouseEvent *e)
{
    switch(mMode) {
        case Scene::StitchMode:
            stitchModeMouseRelease(e);
            break;
        default:
            break;
    }

    Scene::mouseReleaseEvent(e);
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
        undoStack()->push(new SetCellStitch(this, mCurCell, mEditStitch));
    
        mCurCell = 0;
    } else if(!mRubberBand && !mMoving){
        int x = ceil(e->scenePos().x() / defaultSize().width()) - 1;
        int y = ceil(e->scenePos().y() / defaultSize().height()) - 1;

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
