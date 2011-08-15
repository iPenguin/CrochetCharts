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
    : Scene(parent),
    mCenterSymbol(0)
{
}

SceneRows::~SceneRows()
{

}

void SceneRows::setShowChartCenter(bool state)
{
    mShowChartCenter = state;

    if(mStyle == SceneRows::Rounds) {
        if(mShowChartCenter) {
            if(!mCenterSymbol) {
                QPen pen;
                pen.setWidth(5);

                double radius = (mDefaultSize.height() *0.45);
                
                mCenterSymbol = addEllipse(-radius, -radius, radius * 2, radius * 2, pen);
                mCenterSymbol->setToolTip(tr("Chart Center"));
            } else {
                addItem(mCenterSymbol);
            }
        } else {
            removeItem(mCenterSymbol);
        }
    }
}

void SceneRows::addIndicator(Indicator* i)
{
    addItem(i);
    mIndicators.append(i);
}

void SceneRows::removeIndicator(Indicator *i)
{
    removeItem(i);
    mIndicators.removeOne(i);
    update();
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
    removeItem(c);
    for(int i = 0; i < mGrid.count(); ++i) {
        if (mGrid[i].contains(c)) {
            mGrid[i].removeOne(c);
        }
    }

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
   
    if(!fromSave) {
        if(mStyle == SceneRows::Rounds)
            redistributeCells(row);
    }
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

}

void SceneRows::setCellPosition(int row, int column, CrochetCell *c, int columns, bool updateAnchor)
{
    if(mStyle == SceneRows::Rounds) {
        double widthInDegrees = 360.0 / columns;

        double radius = mDefaultSize.height() * (row + 1) + (mDefaultSize.height() *0.5);
        
        double degrees = widthInDegrees*column;
        QPointF finish = calcPoint(radius, degrees, QPointF(0,0));

        qreal delta = mDefaultSize.width() * 0.5;
        if(updateAnchor || c->anchor().isNull())
            c->setAnchor(finish.x() - delta, finish.y());
        c->setPos(finish.x() - delta, finish.y());
        c->setTransform(QTransform().translate(delta,0).rotate(degrees + 90).translate(-delta, 0));
        
    } else {
        c->setPos(column*mDefaultSize.width(), row*mDefaultSize.height());
        if(updateAnchor || c->anchor().isNull())
            c->setAnchor(column*mDefaultSize.width(), row*mDefaultSize.height());
        c->setColor(QColor(Qt::white));
    }
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

void SceneRows::createChart(SceneRows::ChartStyle style, int rows, int cols, QString stitch, QSizeF rowSize)
{
    mStyle = style;
    if(mStyle == SceneRows::Blank) {


    } else {
        mDefaultSize = rowSize;

        for(int i = 0; i < rows; ++i) {
            int pad = 0;
            if(mStyle == SceneRows::Rounds)
                pad = i*8;

            createRow(i, cols + pad, stitch);
        }

        setShowChartCenter(Settings::inst()->value("showChartCenter").toBool());
    }

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

int SceneRows::getClosestRow(QPointF mousePosition)
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

int SceneRows::getClosestColumn(QPointF mousePosition, int row)
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

QPointF SceneRows::calcPoint(double radius, double angleInDegrees, QPointF origin)
{
    // Convert from degrees to radians via multiplication by PI/180
    qreal x = (radius * cos(angleInDegrees * M_PI / 180)) + origin.x();
    qreal y = (radius * sin(angleInDegrees * M_PI / 180)) + origin.y();
    return QPointF(x, y);
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

void SceneRows::updateSelection(QPolygonF selection)
{
    QPainterPath path;
    path.addPolygon(selection);
    setSelectionArea(path);
}

void SceneRows::updateRubberBand(int dx, int dy)
{

    if(mRubberBandStart.isNull())
        return;

    mRubberBandStart.setX(mRubberBandStart.x() - dx);
    mRubberBandStart.setY(mRubberBandStart.y() - dy);
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

void SceneRows::mousePressEvent(QGraphicsSceneMouseEvent *e)
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
        case SceneRows::PositionMode:
            positionModeMousePress(e);
            break;
        case SceneRows::AngleMode:
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

void SceneRows::mouseMoveEvent(QGraphicsSceneMouseEvent *e)
{

    switch(mMode) {
        case SceneRows::StitchMode:
            stitchModeMouseMove(e);
            break;
        case SceneRows::ColorMode:
            colorModeMouseMove(e);
            break;
        case SceneRows::PositionMode:
            Scene::mouseMoveEvent(e);
            break;
        case SceneRows::AngleMode:
            angleModeMouseMove(e);
            break;
        case SceneRows::StretchMode:
            stretchModeMouseMove(e);
            break;
        case SceneRows::IndicatorMode:
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

void SceneRows::mouseReleaseEvent(QGraphicsSceneMouseEvent *e)
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
        case SceneRows::StitchMode:
            stitchModeMouseRelease(e);
            break;
        case SceneRows::ColorMode:
            colorModeMouseRelease(e);
            break;
        case SceneRows::PositionMode:
            positionModeMouseRelease(e);
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

void SceneRows::positionModeMousePress(QGraphicsSceneMouseEvent* e)
{
    Q_UNUSED(e);
    if(selectedItems().count() <= 0)
        return;

    foreach(QGraphicsItem *item, selectedItems()) {
        mOldPositions.insert(item, item->pos());
    }

}

void SceneRows::positionModeMouseRelease(QGraphicsSceneMouseEvent* e)
{
    
    //qreal diff = e->buttonDownPos(Qt::LeftButton).manhattanLength() - e->scenePos().manhattanLength();
    //if(diff >= QApplication::startDragDistance()) {
    if(selectedItems().count() > 0 && mOldPositions.count() > 0) {
        mUndoStack.beginMacro("move items");
        foreach(QGraphicsItem *item, selectedItems()) {
            if(mOldPositions.contains(item)) {
                QPointF oldPos = mOldPositions.value(item);
                mUndoStack.push(new SetItemCoordinates(this, item, oldPos, item->pos()));
            }
        }
        mUndoStack.endMacro();
        mOldPositions.clear();
    }
}

void SceneRows::stitchModeMouseMove(QGraphicsSceneMouseEvent* e)
{
    if(e->buttons() != Qt::LeftButton)
        return;
    
    if(!mCurCell)
        return;
    
    if(mCurCell->name() != mEditStitch)
        mUndoStack.push(new SetCellStitch(this, mCurCell, mEditStitch));
}

void SceneRows::stitchModeMouseRelease(QGraphicsSceneMouseEvent* e)
{
    //FIXME: foreach(stitch in selection()) create an undo group event.
    if(mCurCell) {
        
    if(mCurCell->name() != mEditStitch)
        mUndoStack.push(new SetCellStitch(this, mCurCell, mEditStitch));
    
        mCurCell = 0;
    } else if(!mRubberBand){
        //FIXME: combine getClosestRow & getClosestColumn into 1 function returning a QPoint.
        int x = 0;
        int y = 0;
        if(mStyle == SceneRows::Rounds) {
            y = getClosestRow(e->scenePos());
            //FIXME: the row has to be passed in because getClosestRow modifies the row
            x = getClosestColumn(e->scenePos(), y);
        } else if (mStyle == SceneRows::Rows) {
            x = ceil(e->scenePos().x() / mDefaultSize.width()) - 1;
            y = ceil(e->scenePos().y() / mDefaultSize.height()) - 1;
        }

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
