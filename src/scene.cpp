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

Scene::Scene(QObject* parent)
    : QGraphicsScene(parent),
    mCurCell(0),
    mCellStartPos(QPointF(0,0)),
    mLeftButtonDownPos(QPointF(0,0)),
    mCurIndicator(0),
    mDiff(QSizeF(0,0)),
    mRubberBand(0),
    mRubberBandStart(QPointF(0,0)),
    mMoving(false),
    mMode(Scene::StitchEdit),
    mEditStitch("ch"),
    mEditFgColor(QColor(Qt::black)),
    mEditBgColor(QColor(Qt::white)),
    mScale(1.0),
    mOldScale(1.0),
    mAngle(0.0),
    mOrigin(0,0),
    mRowSpacing(9),
    mDefaultSize(QSizeF(32.0, 96.0)),
    mRowLine(0)
{
    mPivotPt = QPointF(mDefaultSize.width()/2, 0);
}

Scene::~Scene()
{
    foreach(QGraphicsItem* i, items()) {
        delete i;
    }
    items().clear();

}

QStringList Scene::modes()
{
    QStringList modes;
    modes << tr("Stitch Edit") << tr("Color Edit") << tr("Row Edit")
            << tr("Rotation Edit") << tr("Scale Edit") << tr("Indicator Edit");
    return modes;
}

void Scene::initDemoBackground()
{

    if(Settings::inst()->isDemoVersion()) {

        if(mDemoItems.count() > 0) {
            foreach(QGraphicsItem* i, mDemoItems) {
                delete i;
            }
            mDemoItems.clear();
        }
        
        double fontSize = 32.0;
        QFont demoFont = QFont();
        demoFont.setPointSize(fontSize);
        QString demoString = AppInfo::inst()->demoString;

        QFontMetrics fm = QFontMetrics(demoFont);
        double stringWidth = fm.width(demoString);

        QGraphicsSimpleTextItem* demoText;
        QRectF rect = sceneRect();
        double demoRows = rect.height() / fontSize;
        demoRows = demoRows /2.0;

        double demoCols = rect.width() / stringWidth;

        for(int c = 0; c < ceil(demoCols); ++c) {
            for(int i = 0; i < ceil(demoRows); ++i) {
                demoText = addSimpleText(demoString, demoFont);
                demoText->setBrush(QBrush(QColor("#ECECEC")));
                QPointF point = QPointF(rect.left() + c * stringWidth, rect.top() + i * (2 * fontSize));
                demoText->setPos(point);
                demoText->setZValue(-1);
                mDemoItems.append(demoText);
            }
        }

        //restore original rect. letting the demo text overflow off the scene.
        setSceneRect(rect);
    }
}

int Scene::rowCount()
{
    return rows.count();
}

int Scene::columnCount(int row)
{
    if(row >= rows.count())
        return 0;

    return rows[row].count();
}

int Scene::maxColumnCount()
{
    int max = 0;
    for(int i = 0; i < rowCount(); ++i) {
        int cols = columnCount(i);
        if(cols > max)
            max = cols;
    }
    return max;
}

void Scene::removeCell(CrochetCell* c)
{
    removeItem(c);
    removeFromRows(c);
}

void Scene::removeFromRows(CrochetCell* c)
{
    for(int y = 0; y < rows.count(); ++y) {
        if(rows[y].contains(c)) {
            rows[y].removeOne(c);
            if(rows[y].count() == 0)
                rows.removeAt(y);
            break;
        }
    }
}

void Scene::addIndicator(Indicator* i)
{
    addItem(i);
    mIndicators.append(i);
}

void Scene::removeIndicator(Indicator* i)
{
    removeItem(i);
    mIndicators.removeOne(i);
}

void Scene::addCell(CrochetCell* c)
{
    addItem(c);

    connect(c, SIGNAL(stitchChanged(QString,QString)), SIGNAL(stitchChanged(QString,QString)));
    connect(c, SIGNAL(colorChanged(QString,QString)), SIGNAL(colorChanged(QString,QString)));
}

void Scene::updateRubberBand(int dx, int dy)
{

    if(mRubberBandStart.isNull())
        return;

    mRubberBandStart.setX(mRubberBandStart.x() - dx);
    mRubberBandStart.setY(mRubberBandStart.y() - dy);
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
        undoStack()->beginMacro("Remove items");
        foreach(QGraphicsItem* item, items) {
            CrochetCell* c = qgraphicsitem_cast<CrochetCell*>(item);
            if(c) {
                undoStack()->push(new RemoveCell(this, c));
            } else {
                Indicator* i = qgraphicsitem_cast<Indicator*>(item);
                undoStack()->push(new RemoveIndicator(this, i));
            }
        }
        undoStack()->endMacro();
    }
        
    QGraphicsScene::keyReleaseEvent(keyEvent);
}

void Scene::mousePressEvent(QGraphicsSceneMouseEvent* e)
{
    
    QGraphicsScene::mousePressEvent(e);
    
    mLeftButtonDownPos = e->buttonDownPos(Qt::LeftButton);

    QGraphicsItem* gi = itemAt(e->scenePos());
    if(gi) {
        
        CrochetCell* c = qgraphicsitem_cast<CrochetCell*>(gi);

        if(c) {
            
            mCurCell = c;
            mCellStartPos = mCurCell->pos();
            mDiff = QSizeF(e->scenePos().x() - mCellStartPos.x(), e->scenePos().y() - mCellStartPos.y());
        } else {
            
            Indicator* i = qgraphicsitem_cast<Indicator*>(gi);

            if(i) {
                
                mCurIndicator = i;
                mCellStartPos = i->pos();
            }
        }
    }

    mMoving = false;
    
    switch(mMode) {
        case Scene::StitchEdit:
            stitchModeMousePress(e);
            break;
        case Scene::RotationEdit:
            angleModeMousePress(e);
            break;
        case Scene::ScaleEdit:
            stretchModeMousePress(e);
            break;
        case Scene::RowEdit:
            rowEditMousePress(e);
            return;
        default:
            break;
    }
    
    if(e->buttons() != Qt::LeftButton)
        return;
    
    if(selectedItems().count() <= 0) {
        
        ChartView* view = qobject_cast<ChartView*>(parent());

        if(!mRubberBand)
            mRubberBand = new QRubberBand(QRubberBand::Rectangle, view);

        mRubberBandStart = view->mapFromScene(e->scenePos());

        mRubberBand->setGeometry(QRect(mRubberBandStart.toPoint(), QSize()));
        mRubberBand->show();
    } else {
        
    //Track object movement on scene.
        foreach(QGraphicsItem* item, selectedItems()) {
            mOldPositions.insert(item, item->pos());
        }
    }
    
}

void Scene::mouseMoveEvent(QGraphicsSceneMouseEvent* e)
{

    switch(mMode) {
        case Scene::StitchEdit:
            stitchModeMouseMove(e);
            break;
        case Scene::ColorEdit:
            colorModeMouseMove(e);
            break;
        case Scene::IndicatorEdit:
            indicatorModeMouseMove(e);
            break;
        case Scene::RotationEdit:
            angleModeMouseMove(e);
            return;
        case Scene::ScaleEdit:
            stretchModeMouseMove(e);
            return;
        case Scene::RowEdit:
            rowEditMouseMove(e);
            return;
        default:
            break;
    }

    if(mRubberBand) {
        ChartView* view = qobject_cast<ChartView*>(parent());
        QRect rect = QRect(mRubberBandStart.toPoint(), view->mapFromScene(e->scenePos()));

        mRubberBand->setGeometry(rect.normalized());
    }
    
    qreal diff = (e->buttonDownPos(Qt::LeftButton)- e->scenePos()).manhattanLength();
    if(diff >= QApplication::startDragDistance()) {
        mMoving = true;
        QGraphicsScene::mouseMoveEvent(e);
    }
}

void Scene::mouseReleaseEvent(QGraphicsSceneMouseEvent* e)
{

    if(mRubberBand) {
        ChartView* view = qobject_cast<ChartView*>(parent());
        QRect rect = QRect(mRubberBandStart.toPoint(), view->mapFromScene(e->scenePos()));

        QPolygonF p = view->mapToScene(rect.normalized());
        QPainterPath path;
        path.addPolygon(p);
        setSelectionArea(path);

        mRubberBand->hide();
    }
    
    if((selectedItems().count() > 0 && mOldPositions.count() > 0) && mMoving) {
        undoStack()->beginMacro("move items");
        foreach(QGraphicsItem* item, selectedItems()) {
            if(mOldPositions.contains(item)) {
                QPointF oldPos = mOldPositions.value(item);
                undoStack()->push(new SetItemCoordinates(this, item, oldPos));
            }
        }
        undoStack()->endMacro();
        mOldPositions.clear();
    }

    switch(mMode) {
        case Scene::StitchEdit:
            stitchModeMouseRelease(e);
            break;
        case Scene::ColorEdit:
            colorModeMouseRelease(e);
            break;
        case Scene::IndicatorEdit:
            indicatorModeMouseRelease(e);
            break;
        case Scene::RotationEdit:
            angleModeMouseRelease(e);
            break;
        case Scene::ScaleEdit:
            stretchModeMouseRelease(e);
            break;
        case Scene::RowEdit:
            rowEditMouseRelease(e);
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

        initDemoBackground();
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
        undoStack()->push(new SetCellColor(this, mCurCell, mEditBgColor));
}

void Scene::indicatorModeMouseMove(QGraphicsSceneMouseEvent* e)
{
    if(e->buttons() != Qt::LeftButton)
        return;

    mMoving = true;

    if(!mCurIndicator)
        return;

    QPointF start = e->buttonDownScenePos(Qt::LeftButton);

    QPointF delta =  e->scenePos() - start;
    QPointF newPos = mCellStartPos + delta;

    mCurIndicator->setPos(newPos);
}

void Scene::indicatorModeMouseRelease(QGraphicsSceneMouseEvent* e)
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


void Scene::angleModeMousePress(QGraphicsSceneMouseEvent* e)
{
    Q_UNUSED(e);
    if(!mCurCell)
        return;

    mOldAngle = mCurCell->angle();
    mPivotPt = QPointF(mCurCell->stitch()->width()/2, 0);
    mOrigin = mCurCell->mapToScene(mPivotPt);
}

void Scene::angleModeMouseMove(QGraphicsSceneMouseEvent* e)
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

void Scene::angleModeMouseRelease(QGraphicsSceneMouseEvent* e)
{
    Q_UNUSED(e);
    if(!mCurCell)
        return;
    
//FIXME: use a constant pviot point.
    undoStack()->push(new SetCellRotation(this, mCurCell, mOldAngle, mPivotPt));
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

void Scene::stretchModeMouseRelease(QGraphicsSceneMouseEvent* e)
{
    
    Q_UNUSED(e);
    if(!mCurCell)
        return;

    undoStack()->push(new SetCellScale(this, mCurCell, mOldScale, mPivotPt));
    mScale = 1.0;
    mOldScale = 1.0;
}

void Scene::rowEditMousePress(QGraphicsSceneMouseEvent* e)
{

    if(!e->buttons() == Qt::LeftButton)
        return;


    QGraphicsItem* gi = itemAt(e->scenePos());
    mStartCell = qgraphicsitem_cast<CrochetCell*>(gi);
    if(mStartCell) {

        if(mRowSelection.contains(gi) && mRowSelection.last() == gi) {

        } else {
            mRowSelection.clear();
            hideRowLines();
            delete mRowLine;
            
        }
        mRowSelection.append(mStartCell);
    }
    mPreviousCell = mStartCell;

    mRowLine = new QGraphicsLineItem(QLineF(e->scenePos(), e->scenePos()));
    mRowLine->setPen(QPen(QColor(Qt::black), 2));
    addItem(mRowLine);


}

void Scene::rowEditMouseMove(QGraphicsSceneMouseEvent* e)
{
    
    if(!e->buttons() == Qt::LeftButton)
        return;

    if(!mStartCell)
        return;

    QPointF startPt = mRowLine->line().p1();
    
    QGraphicsItem* gi = itemAt(e->scenePos());
    if(gi) {
        CrochetCell* c = qgraphicsitem_cast<CrochetCell*>(gi);
        if(!c)
            return;
        if(!mRowSelection.contains(gi)) {
            mRowSelection.append(gi);
            gi->setSelected(true);

            if(mPreviousCell != c) {
                QGraphicsLineItem* line = addLine(QLineF(mPreviousCell->pos(), c->pos()));
                line->setPen(QPen(QColor(Qt::black), 2));
                mRowLines.append(line);

                mPreviousCell = c;
                startPt = c->pos();
            }
            
        } //else remove from list

    }
    
    if(mRowLine != 0) {
        QLineF newLine(startPt, e->scenePos());
        mRowLine->setLine(newLine);
    }
    
}

void Scene::rowEditMouseRelease(QGraphicsSceneMouseEvent* e)
{
    if(e->buttons() != Qt::LeftButton)
        return;
    
    if(!mStartCell)
        return;
    
    mStartCell = 0;
    delete mRowLine;
    
}

void Scene::createRow(int row)
{
    
    if(selectedItems().count() <= 0)
        return;
    
    QList<CrochetCell*> r;

    foreach(QGraphicsItem* i, mRowSelection) {
        CrochetCell* c = qgraphicsitem_cast<CrochetCell*>(i);
        removeFromRows(c);
        c->useAlternateRenderer((rows.count() % 2));
        r.append(c);
    }
    rows.append(r);

}
void Scene::updateRow(int row)
{
    //FIXME: this overlaps the createRow code.
    if(selectedItems().count() <= 0)
        return;
qDebug() << "update start";
    QList<CrochetCell*> r;
    
    foreach(QGraphicsItem* i, mRowSelection) {
        CrochetCell* c = qgraphicsitem_cast<CrochetCell*>(i);
        removeFromRows(c);
        c->useAlternateRenderer((row % 2));
        r.append(c);
    }
qDebug() << "remove";    
    rows.removeAt(row);
qDebug() << "insert";
    rows.insert(row, r);
}

QPoint Scene::indexOf(CrochetCell* c)
{
    for(int y = 0; y < rows.count(); ++y) {
        if(rows[y].contains(c)) {
            return QPoint(rows[y].indexOf(c), y);
        }
    }

    return QPoint(-1,-1);
}

void Scene::highlightRow(int row)
{

    if(row >= rows.count())
        return;

    clearSelection();

    foreach(CrochetCell* c, rows[row]) {
        if(c)
            c->setSelected(true);
    }

    emit selectionChanged();
}

void Scene::moveRowDown(int row)
{
    QList<CrochetCell*> r = rows.takeAt(row);
    rows.insert(row + 1, r);
    updateStitchRenderer();
}

void Scene::moveRowUp(int row)
{
    QList<CrochetCell*> r = rows.takeAt(row);
    rows.insert(row - 1, r);
    updateStitchRenderer();
    
}

void Scene::removeRow(int row)
{

    rows.takeAt(row);
    updateStitchRenderer();

}

void Scene::updateStitchRenderer()
{

    for(int i = 0; i < rows.count(); ++i) {
        foreach(CrochetCell* c, rows[i]) {
            c->useAlternateRenderer((i % 2));
        }
    }

}

void Scene::drawRowLines(int row)
{
    if(rows.count() <= row)
        return;

    hideRowLines();

    QPointF start, end;

    int count = rows[row].count();

    QGraphicsItem* prev = rows[row].first();

    for(int i = 0; i < count; ++i) {
        QGraphicsItem* c = rows[row][i];
        if(prev != c) {
            start = prev->pos();
            end = c->pos();

            QGraphicsLineItem *line = addLine(QLineF(start, end));
            line->setPen(QPen(QColor(Qt::black), 2));
            mRowLines.append(line);

            prev = c;
        }
    }

}

void Scene::hideRowLines()
{
    if(mRowLines.count() > 0) {
        foreach(QGraphicsLineItem* i, mRowLines) {
            delete i;
        }
        mRowLines.clear();
    }
}
