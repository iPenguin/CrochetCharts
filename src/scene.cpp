/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "scene.h"

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

Scene::Scene(QObject *parent)
    : QGraphicsScene(parent),
    mCurCell(0),
    mCellStartPos(QPointF(0,0)),
    mLeftButtonDownPos(QPointF(0,0)),
    mCurIndicator(0),
    mDiff(QSizeF(0,0)),
    mRubberBand(0),
    mRubberBandStart(QPointF(0,0)),
    mMoving(false),
    mMode(Scene::StitchMode),
    mEditStitch("ch"),
    mEditFgColor(QColor(Qt::black)),
    mEditBgColor(QColor(Qt::white)),
    mScale(1.0),
    mOldScale(1.0),
    mAngle(0.0),
    mOrigin(0,0),
    mRowSpacing(9),
    mDefaultSize(QSizeF(32.0, 96.0))
{
    mPivotPt = QPointF(mDefaultSize.width()/2, 0);
}

Scene::~Scene()
{
    foreach(QGraphicsItem *i, items()) {
        delete i;
    }
    items().clear();
    mGrid.clear();
}

QStringList Scene::modes()
{
    QStringList modes;
    modes << tr("Stitch Mode") << tr("Color Mode") << tr("Position Mode")
            << tr("Angle Mode") << tr("Stretch Mode") << tr("Indicator Mode");
    return modes;
}

void Scene::initDemoBackground()
{

    if(Settings::inst()->isDemoVersion()) {

        double fontSize = 32.0;
        QFont demoFont = QFont();
        demoFont.setPointSize(fontSize);
        QString demoString = AppInfo::inst()->demoString;

        QFontMetrics fm = QFontMetrics(demoFont);
        double stringWidth = fm.width(demoString);

        QGraphicsSimpleTextItem *demoText;
        QRectF rect = sceneRect();
        double demoRows = rect.height() / fontSize;
        demoRows = demoRows /2.0;

        double demoCols = rect.width() / stringWidth;

        for(int c = 0; c < ceil(demoCols); ++c) {
            for(int i = 0; i < ceil(demoRows); ++i) {
                demoText = addSimpleText(demoString, demoFont);
                demoText->setBrush(QBrush(QColor("#ECECEC")));
                QPointF point = QPointF(rect.left() + c*stringWidth , rect.top() + i*(2*fontSize));
                demoText->setPos(point);
                demoText->setZValue(-1);
            }
        }

        //restore original rect. letting the demo text overflow off the scene.
        setSceneRect(rect);
    }
}

void Scene::addIndicator(Indicator* i)
{
    addItem(i);
    mIndicators.append(i);
}

void Scene::removeIndicator(Indicator *i)
{
    removeItem(i);
    mIndicators.removeOne(i);
}

CrochetCell* Scene::cell(int row, int column)
{
    Q_ASSERT(mGrid.count() > row);
    if(mGrid[row].count() <= column)
        return 0;

    return mGrid[row][column];
}

CrochetCell* Scene::cell(QPoint position)
{
    return cell(position.y(), position.x());
}

void Scene::updateRubberBand(int dx, int dy)
{

    if(mRubberBandStart.isNull())
        return;

    mRubberBandStart.setX(mRubberBandStart.x() - dx);
    mRubberBandStart.setY(mRubberBandStart.y() - dy);
}

QPoint Scene::findGridPosition(CrochetCell* c)
{
    for(int y = 0; y < mGrid.count(); ++y) {
        if(mGrid[y].contains(c)) {
            return QPoint(mGrid[y].indexOf(c), y);
        }
    }
    
    return QPoint();
}

qreal Scene::scenePosToAngle(QPointF pt)
{

    qreal rads = atan2(pt.x(), pt.y());
    qreal angleX = rads * 180 / M_PI;

    return -angleX;
}

void Scene::keyReleaseEvent(QKeyEvent* keyEvent)
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
        
    QGraphicsScene::keyReleaseEvent(keyEvent);
}

void Scene::mousePressEvent(QGraphicsSceneMouseEvent *e)
{
    QGraphicsScene::mousePressEvent(e);

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

    if(e->buttons() != Qt::LeftButton)
        return;

    if(selectedItems().count() <= 0) {
        ChartView *view = qobject_cast<ChartView*>(parent());

        if(!mRubberBand)
            mRubberBand = new QRubberBand(QRubberBand::Rectangle, view);

        mRubberBandStart = view->mapFromScene(e->scenePos());

        mRubberBand->setGeometry(QRect(mRubberBandStart.toPoint(), QSize()));
        mRubberBand->show();
    } else {
    //Track object movement on scene.
        foreach(QGraphicsItem *item, selectedItems()) {
            mOldPositions.insert(item, item->pos());
        }
    }

    mMoving = false;

    switch(mMode) {
        case Scene::AngleMode:
            angleModeMousePress(e);
            break;
        case Scene::StretchMode:
            stretchModeMousePress(e);
            break;
        default:
            break;
    }
}

void Scene::mouseMoveEvent(QGraphicsSceneMouseEvent *e)
{

    if(mRubberBand) {
        ChartView *view = qobject_cast<ChartView*>(parent());
        QRect rect = QRect(mRubberBandStart.toPoint(), view->mapFromScene(e->scenePos()));

        mRubberBand->setGeometry(rect.normalized());
    }

    switch(mMode) {
        case Scene::ColorMode:
            colorModeMouseMove(e);
            break;
        case Scene::IndicatorMode:
            indicatorModeMouseMove(e);
            break;
        case Scene::AngleMode:
            angleModeMouseMove(e);
            return;
        case Scene::StretchMode:
            stretchModeMouseMove(e);
            return;
        default:
            break;
    }
    
    qreal diff = (e->buttonDownPos(Qt::LeftButton)- e->scenePos()).manhattanLength();
    if(diff >= QApplication::startDragDistance()) {
        mMoving = true;
        QGraphicsScene::mouseMoveEvent(e);
    }
}

void Scene::mouseReleaseEvent(QGraphicsSceneMouseEvent *e)
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
    
    if((selectedItems().count() > 0 && mOldPositions.count() > 0) && mMoving) {
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

    switch(mMode) {
        case Scene::ColorMode:
            colorModeMouseRelease(e);
            break;
        case Scene::IndicatorMode:
            indicatorModeMouseRelease(e);
            break;
        case Scene::AngleMode:
            angleModeMouseRelease(e);
            break;
        case Scene::StretchMode:
            stretchModeMouseRelease(e);
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

    QGraphicsScene::mouseReleaseEvent(e);
    if(mMoving) {
        //update the size of the scene rect based on where the items are on the scene.
        QRectF ibr = itemsBoundingRect();
        QRectF sbr = sceneRect();
        QRectF final;

        final.setBottom((ibr.bottom() >= sbr.bottom()) ? ibr.bottom() : sbr.bottom());
        final.setTop((ibr.top() <= sbr.top()) ? ibr.top() : sbr.top());
        final.setLeft((ibr.left() <= sbr.left()) ? ibr.left() : sbr.left());
        final.setRight((ibr.right() >= sbr.right()) ? ibr.right() : sbr.right());

        setSceneRect(final);

        mMoving = false;
    }

}

void Scene::colorModeMouseMove(QGraphicsSceneMouseEvent* e)
{
    if(e->buttons() != Qt::LeftButton)
        return;

    if(!mCurCell)
        return;
/*
 *FIXME: if the user isn't dragging a stitch we should be painting with color.
    if(mCurCell->color() != mEditBgColor)
        mUndoStack.push(new SetCellColor(this, mCurCell, mEditBgColor));
*/
}

void Scene::colorModeMouseRelease(QGraphicsSceneMouseEvent* e)
{
    Q_UNUSED(e);
    if(!mCurCell)
        return;

    if(mCurCell->color() != mEditBgColor)
        mUndoStack.push(new SetCellColor(this, mCurCell, mEditBgColor));
}

void Scene::indicatorModeMouseMove(QGraphicsSceneMouseEvent *e)
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

void Scene::indicatorModeMouseRelease(QGraphicsSceneMouseEvent *e)
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

    } else {
        mCurIndicator->setTextInteractionFlags(Qt::TextEditorInteraction);
    }

}


void Scene::angleModeMousePress(QGraphicsSceneMouseEvent *e)
{
    Q_UNUSED(e);
    if(!mCurCell)
        return;

    mOldAngle = mCurCell->angle();
    mPivotPt = QPointF(mCurCell->stitch()->width()/2, 0);
    mOrigin = mCurCell->mapToScene(mPivotPt);
}

void Scene::angleModeMouseMove(QGraphicsSceneMouseEvent *e)
{
    if(!mCurCell)
        return;

    
    QPointF first = e->buttonDownScenePos(Qt::LeftButton);
    QPointF second = e->scenePos();

    QPointF rel1 = first - mOrigin;
    QPointF rel2 = second - mOrigin;
    qreal angle1 = scenePosToAngle(rel1);
    qreal angle2 = scenePosToAngle(rel2);

    mAngle = mOldAngle - (angle1 - angle2);
    mCurCell->setRotation(mAngle, mPivotPt);

}

void Scene::angleModeMouseRelease(QGraphicsSceneMouseEvent *e)
{
    Q_UNUSED(e);
    if(!mCurCell)
        return;
    
//FIXME: use a constant pviot point.
    mUndoStack.push(new SetCellRotation(this, mCurCell, mOldAngle, mPivotPt));
    mOldAngle = 0;
}

void Scene::stretchModeMousePress(QGraphicsSceneMouseEvent* e)
{
    Q_UNUSED(e);
    if(!mCurCell)
        return;

    mOldScale = mCurCell->scale();
    mPivotPt = QPointF(mCurCell->stitch()->width()/2, 0);
    mCurScale = mCurCell->scale();
}

void Scene::stretchModeMouseMove(QGraphicsSceneMouseEvent* e)
{
    Q_UNUSED(e);
    if(!mCurCell)
        return;

    QPointF delta = e->buttonDownScenePos(Qt::LeftButton) - e->scenePos();

    mScale = mCurScale - (delta.y()/mCurCell->origHeight());
    mCurCell->setScale(mScale, mPivotPt);
}

void Scene::stretchModeMouseRelease(QGraphicsSceneMouseEvent *e)
{
    Q_UNUSED(e);
    if(!mCurCell)
        return;

    mUndoStack.push(new SetCellScale(this, mCurCell, mOldScale, mPivotPt));
    mScale = 1.0;
    mOldScale = 1.0;
}
