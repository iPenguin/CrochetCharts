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

void SceneBlank::addIndicator(Indicator* i)
{
    addItem(i);
    mIndicators.append(i);
}

void SceneBlank::removeIndicator(Indicator *i)
{
    removeItem(i);
    mIndicators.removeOne(i);
    update();
}

CrochetCell* SceneBlank::cell(int row, int column)
{
    Q_ASSERT(mGrid.count() > row);
    if(mGrid[row].count() <= column)
        return 0;

    return mGrid[row][column];
}

CrochetCell* SceneBlank::cell(QPoint position)
{
    return cell(position.y(), position.x());
}

void SceneBlank::removeCell(CrochetCell *c)
{
    removeItem(c);
    for(int i = 0; i < mGrid.count(); ++i) {
        if (mGrid[i].contains(c)) {
            mGrid[i].removeOne(c);
        }
    }

}

int SceneBlank::rowCount()
{
    return mGrid.count();
}

int SceneBlank::columnCount(int row)
{
    if(mGrid.count() <= row)
        return 0;
    return mGrid[row].count();
}

void SceneBlank::appendCell(int row, CrochetCell *c, bool fromSave)
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

void SceneBlank::addCell(QPoint p, CrochetCell* c)
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

}

void SceneBlank::createChart(int rows, int cols, QString stitch, QSizeF rowSize)
{

    initDemoBackground();
}

void SceneBlank::createRow(int row, int columns, QString stitch)
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

int SceneBlank::getClosestRow(QPointF mousePosition)
{
    //double radius = mDefaultSize.height() * (row + 1) + (mDefaultSize.height() *0.5);
    qreal radius = sqrt(mousePosition.x()*mousePosition.x() + mousePosition.y()*mousePosition.y());

    qreal temp = radius - (mDefaultSize.height() *0.5);
    qreal temp2 = temp / mDefaultSize.height();
    
    int row = round(temp2 - 1);
    if(row < 0)
        row = 0;
    if(row >= mGrid.count()) {
        row = mGrid.count();

        QList<CrochetCell*> r;
        mGrid.append(r);
    }

    return row;
}

int SceneBlank::getClosestColumn(QPointF mousePosition, int row)
{
    /*
              |
          -,- | +,-
        ------+------
          -,+ | +,+
              |
    */
    qreal tanx = mousePosition.y() / mousePosition.x();
    qreal rads = atan(tanx);
    qreal angleX = rads * 180 / M_PI;
    qreal angle = 0.0;
    if (mousePosition.x() >= 0 && mousePosition.y() >= 0)
        angle = angleX;
    else if(mousePosition.x() <= 0 && mousePosition.y() >= 0)
        angle = 180 + angleX;
    else if(mousePosition.x() <= 0 && mousePosition.y() <= 0)
        angle = 180 + angleX;
    else if(mousePosition.x() >= 0 && mousePosition.y() <= 0)
        angle = 360 + angleX;

    qreal degreesPerPos = 360.0 / mGrid[row].count();

    return ceil(angle / degreesPerPos);
}

QPointF SceneBlank::calcPoint(double radius, double angleInDegrees, QPointF origin)
{
    // Convert from degrees to radians via multiplication by PI/180
    qreal x = (radius * cos(angleInDegrees * M_PI / 180)) + origin.x();
    qreal y = (radius * sin(angleInDegrees * M_PI / 180)) + origin.y();
    return QPointF(x, y);
}

void SceneBlank::stitchUpdated(QString oldSt, QString newSt)
{
    Q_UNUSED(newSt);
    
    if(oldSt.isEmpty() || oldSt.isNull())
        return;
    
    CrochetCell *c = qobject_cast<CrochetCell*>(sender());
    if(!c)
        return;

    QPoint pos = findGridPosition(c);
    
}

void SceneBlank::updateSelection(QPolygonF selection)
{
    QPainterPath path;
    path.addPolygon(selection);
    setSelectionArea(path);
}

void SceneBlank::updateRubberBand(int dx, int dy)
{

    if(mRubberBandStart.isNull())
        return;

    mRubberBandStart.setX(mRubberBandStart.x() - dx);
    mRubberBandStart.setY(mRubberBandStart.y() - dy);
}

QPoint SceneBlank::findGridPosition(CrochetCell* c)
{
    for(int y = 0; y < mGrid.count(); ++y) {
        if(mGrid[y].contains(c)) {
            return QPoint(mGrid[y].indexOf(c), y);
        }
    }
    
    return QPoint();
}

qreal SceneBlank::scenePosToAngle(QPointF pt)
{

    qreal rads = atan2(pt.x(), pt.y());
    qreal angleX = rads * 180 / M_PI;
    
    return -angleX;
}

void SceneBlank::setCellPosition(int row, int column, CrochetCell* c, int columns, bool updateAnchor)
{

}

void SceneBlank::keyReleaseEvent(QKeyEvent* keyEvent)
{
    if(keyEvent->key() == Qt::Key_Delete) {
        QList<QGraphicsItem*> items = selectedItems();
        foreach(QGraphicsItem *item, items) {
            CrochetCell *c = qgraphicsitem_cast<CrochetCell*>(item);
            if(c) {
                undoStack()->push(new RemoveCell(this, c));
            } else {
                Indicator *i = qgraphicsitem_cast<Indicator*>(item);
                undoStack()->push(new RemoveIndicator(this, i));
            }
        }
    }
        
    Scene::keyReleaseEvent(keyEvent);
}

void SceneBlank::mousePressEvent(QGraphicsSceneMouseEvent *e)
{
    Scene::mousePressEvent(e);

    QGraphicsItem *gi = itemAt(e->scenePos());
    CrochetCell *c = qgraphicsitem_cast<CrochetCell*>(gi);

    mLeftButtonDownPos = e->buttonDownPos(Qt::LeftButton);
    
    if(c) {
        mCurCell = c;
        mCellStartPos = mCurCell->pos();
        mDiff = QSizeF(e->scenePos().x() - mCellStartPos.x(), e->scenePos().y() - mCellStartPos.y());
    } else {
        Indicator *i = qgraphicsitem_cast<Indicator*>(gi);
        
        if(i) {
            mCurIndicator = i;
            mCellStartPos = i->pos();
        }
    }
    
    switch(mMode) {
        case SceneBlank::AngleMode:
            angleModeMousePress(e);
            break;
        default:
            break;
    }

    if(e->buttons() != Qt::LeftButton)
        return;

    if(selectedItems().count() <= 0) {
        ChartView *view = qobject_cast<ChartView*>(parent());

        if(!mRubberBand)
            mRubberBand = new QRubberBand(QRubberBand::Rectangle, view);

        mRubberBandStart = view->mapFromScene(e->scenePos());

        mRubberBand->setGeometry(QRect(mRubberBandStart.toPoint(), QSize()));
        mRubberBand->show();
    }
}

void SceneBlank::mouseMoveEvent(QGraphicsSceneMouseEvent *e)
{

    switch(mMode) {
        case SceneBlank::StitchMode:
            stitchModeMouseMove(e);
            break;
        case SceneBlank::ColorMode:
            colorModeMouseMove(e);
            break;
        case SceneBlank::AngleMode:
            angleModeMouseMove(e);
            break;
        case SceneBlank::StretchMode:
            stretchModeMouseMove(e);
            break;
        case SceneBlank::IndicatorMode:
            indicatorModeMouseMove(e);
            Scene::mouseMoveEvent(e);
            break;
        default:
            break;
    }

    if(mRubberBand) {
        ChartView *view = qobject_cast<ChartView*>(parent());
        QRect rect = QRect(mRubberBandStart.toPoint(), view->mapFromScene(e->scenePos()));

        mRubberBand->setGeometry(rect.normalized());
    } 
    
}

void SceneBlank::mouseReleaseEvent(QGraphicsSceneMouseEvent *e)
{

    if(mRubberBand) {
        ChartView *view = qobject_cast<ChartView*>(parent());
        QRect rect = QRect(mRubberBandStart.toPoint(), view->mapFromScene(e->scenePos()));

        QPolygonF p = view->mapToScene(rect.normalized());
        QPainterPath path;
        path.addPolygon(p);
        setSelectionArea(path);

        mRubberBand->hide();

    }

    
    switch(mMode) {
        case SceneBlank::StitchMode:
            stitchModeMouseRelease(e);
            break;
        case SceneBlank::ColorMode:
            colorModeMouseRelease(e);
            break;
        case SceneBlank::AngleMode:
            angleModeMouseRelease(e);
            break;
        case SceneBlank::IndicatorMode:
            indicatorModeMouseRelease(e);
            break;
        default:
            break;
    }

    if(mCurCell) {
        mDiff = QSizeF(0,0);
        mCellStartPos = QPointF(0,0);
        mCurCell = 0;
    }

    if(mCurIndicator) {
        mCellStartPos = QPoint(0,0);
        mCurIndicator = 0;
    }
    mLeftButtonDownPos = QPointF(0,0);
    
    delete mRubberBand;
    mRubberBand = 0;

    Scene::mouseReleaseEvent(e);
}

void SceneBlank::colorModeMouseMove(QGraphicsSceneMouseEvent* e)
{
    if(e->buttons() != Qt::LeftButton)
        return;
    
    if(!mCurCell)
        return;
    
    if(mCurCell->color() != mEditBgColor)
        mUndoStack.push(new SetCellColor(this, mCurCell, mEditBgColor));
}

void SceneBlank::colorModeMouseRelease(QGraphicsSceneMouseEvent* e)
{
    Q_UNUSED(e);
    if(!mCurCell)
        return;

    if(mCurCell->color() != mEditBgColor)
        mUndoStack.push(new SetCellColor(this, mCurCell, mEditBgColor));
}

void SceneBlank::stitchModeMouseMove(QGraphicsSceneMouseEvent* e)
{
    if(e->buttons() != Qt::LeftButton)
        return;
    
    if(!mCurCell)
        return;
    
    if(mCurCell->name() != mEditStitch)
        mUndoStack.push(new SetCellStitch(this, mCurCell, mEditStitch));
}

void SceneBlank::stitchModeMouseRelease(QGraphicsSceneMouseEvent* e)
{
    //FIXME: foreach(stitch in selection()) create an undo group event.
    if(mCurCell) {
        
    if(mCurCell->name() != mEditStitch)
        mUndoStack.push(new SetCellStitch(this, mCurCell, mEditStitch));
    
        mCurCell = 0;
    } else if(!mRubberBand){


    }
}

void SceneBlank::angleModeMousePress(QGraphicsSceneMouseEvent *e)
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

void SceneBlank::angleModeMouseMove(QGraphicsSceneMouseEvent *e)
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

void SceneBlank::angleModeMouseRelease(QGraphicsSceneMouseEvent *e)
{
    Q_UNUSED(e);
    mCurCellRotation = 0;
}

void SceneBlank::stretchModeMouseMove(QGraphicsSceneMouseEvent* e)
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

void SceneBlank::indicatorModeMouseMove(QGraphicsSceneMouseEvent *e)
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

void SceneBlank::indicatorModeMouseRelease(QGraphicsSceneMouseEvent *e)
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
