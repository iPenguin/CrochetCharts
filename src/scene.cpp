/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "scene.h"

#include "crochetcell.h"

#include <QFontMetrics>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsSceneEvent>
#include <QApplication>
#include <QClipboard>

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

static void qNormalizeAngle(qreal &angle)
{
    while (angle < 0.0)
        angle += 360.0;
    while (angle > 360.0)
        angle -= 360.0;
}

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
    mIsRubberband(false),
    mHasSelection(false),
    mSnapTo(false),
    mMode(Scene::StitchEdit),
    mEditStitch("ch"),
    mEditFgColor(QColor(Qt::black)),
    mEditBgColor(QColor(Qt::white)),
    mOldScale(QPointF(1.0, 1.0)),
    mPrevScale(QPointF(1.0,1.0)),
    mAngle(0.0),
    mOrigin(0,0),
    mRowSpacing(9),
    mDefaultSize(QSizeF(32.0, 96.0)),
    mDefaultStitch("ch"),
    mRowLine(0),
    mCenterSymbol(0)
{
    mPivotPt = QPointF(mDefaultSize.width()/2, mDefaultSize.height());
    initDemoBackground();
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

CrochetCell* Scene::cell(int row, int column)
{
    if(row >= grid.count())
        return 0;
    if(column >= grid[row].count())
        return 0;

    return grid[row][column];
}

CrochetCell* Scene::cell(QPoint position)
{
    return cell(position.y(), position.x());
}

int Scene::rowCount()
{
    return grid.count();
}

int Scene::columnCount(int row)
{
    if(row >= grid.count())
        return 0;

    return grid[row].count();
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

void Scene::addItem(QGraphicsItem* item)
{
    switch(item->type()) {
        case CrochetCell::Type: {
            QGraphicsScene::addItem(item);
            CrochetCell* c = qgraphicsitem_cast<CrochetCell*>(item);
            connect(c, SIGNAL(stitchChanged(QString,QString)), SIGNAL(stitchChanged(QString,QString)));
            connect(c, SIGNAL(colorChanged(QString,QString)), SIGNAL(colorChanged(QString,QString)));
            break;
        }
        case Indicator::Type: {
            QGraphicsScene::addItem(item);
            Indicator* i = qgraphicsitem_cast<Indicator*>(item);
            mIndicators.append(i);
            break;
        }
        case QGraphicsItemGroup::Type: {
            QGraphicsScene::addItem(item);
            QGraphicsItemGroup* group = qgraphicsitem_cast<QGraphicsItemGroup*>(item);
            mGroups.append(group);
            break;
        }
        default:
            qWarning() << "addItem: unknown type";
            break;
    }
}

void Scene::removeItem(QGraphicsItem* item)
{

    switch(item->type()) {
        case CrochetCell::Type: {
            QGraphicsScene::removeItem(item);
            CrochetCell* c = qgraphicsitem_cast<CrochetCell*>(item);
            removeFromRows(c);
            break;
        }
        case Indicator::Type: {
            QGraphicsScene::removeItem(item);
            Indicator* i = qgraphicsitem_cast<Indicator*>(item);
            mIndicators.removeOne(i);
            break;
        }
        case QGraphicsItemGroup::Type: {
            QGraphicsScene::removeItem(item);
            QGraphicsItemGroup* group = qgraphicsitem_cast<QGraphicsItemGroup*>(item);
            mGroups.removeOne(group);
            break;
        }
        default:
            qWarning() << "removeItem: unknown type";
            break;
    }

}

void Scene::removeFromRows(CrochetCell* c)
{
    for(int y = 0; y < grid.count(); ++y) {
        if(grid[y].contains(c)) {
            grid[y].removeOne(c);
            if(grid[y].count() == 0)
                grid.removeAt(y);
            break;
        }
    }
}

void Scene::updateRubberBand(int dx, int dy)
{

    if(mRubberBandStart.isNull())
        return;

    mRubberBandStart.setX(mRubberBandStart.x() - dx);
    mRubberBandStart.setY(mRubberBandStart.y() - dy);
}

void Scene::keyReleaseEvent(QKeyEvent* keyEvent)
{
    if(keyEvent->key() == Qt::Key_Delete || keyEvent->key() == Qt::Key_Backspace) {
        QList<QGraphicsItem*> items = selectedItems();
        undoStack()->beginMacro("remove items");
        foreach(QGraphicsItem* item, items) {

            switch(item->type()) {
                case CrochetCell::Type: {
                    CrochetCell* c = qgraphicsitem_cast<CrochetCell*>(item);
                    undoStack()->push(new RemoveCell(this, c));
                    break;
                }
                case Indicator::Type: {
                    Indicator* i = qgraphicsitem_cast<Indicator*>(item);
                    undoStack()->push(new RemoveIndicator(this, i));
                    break;
                }
                case QGraphicsItemGroup::Type: {
                    QGraphicsItemGroup* group = qgraphicsitem_cast<QGraphicsItemGroup*>(item);
                    undoStack()->push(new RemoveGroup(this, group));
                    break;
                }
                default:
                    qWarning() << "keyReleaseEvent: unkown type";
                    break;
            }
        }
        undoStack()->endMacro();
    }

    QGraphicsScene::keyReleaseEvent(keyEvent);
}

void Scene::mousePressEvent(QGraphicsSceneMouseEvent* e)
{
    if(selectedItems().count() > 0)
        mHasSelection = true;
    
    QGraphicsScene::mousePressEvent(e);
    
    mLeftButtonDownPos = e->buttonDownPos(Qt::LeftButton);

    mMoving = false;
    mIsRubberband = false;
    
    QGraphicsItem* gi = itemAt(e->scenePos());
    if(gi) {
        
        switch(gi->type()) {
            case CrochetCell::Type: {
                CrochetCell* c = qgraphicsitem_cast<CrochetCell*>(gi);
                mCurCell = c;
                mCellStartPos = mCurCell->scenePos();
                mDiff = QSizeF(e->scenePos().x() - mCellStartPos.x(), e->scenePos().y() - mCellStartPos.y());
                break;
            }
            case Indicator::Type: {
                Indicator* i = qgraphicsitem_cast<Indicator*>(gi);
            
                mCurIndicator = i;
                mCellStartPos = i->scenePos();
                break;
            }
            case QGraphicsEllipseItem::Type: {
                mMoving = true;
                break;
            }
            case QGraphicsItemGroup::Type: {
                mMoving = true;
                break;
            }
            default:
                qWarning() << "mousePress: Unknown object type.";
                break;
        }
    }
    
    switch(mMode) {
        case Scene::StitchEdit:
            stitchModeMousePress(e);
            break;
        case Scene::RotationEdit:
            angleModeMousePress(e);
            break;
        case Scene::ScaleEdit:
            scaleModeMousePress(e);
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
            mOldPositions.insert(item, item->scenePos());
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
            break;
        case Scene::ScaleEdit:
            scaleModeMouseMove(e);
            break;
        case Scene::RowEdit:
            rowEditMouseMove(e);
            return;
        default:
            break;
    }

    qreal diff = (e->buttonDownScenePos(Qt::LeftButton)- e->scenePos()).manhattanLength();

    if(diff >= QApplication::startDragDistance()) {

        if(!mCurCell && !mMoving) {
            if(mRubberBand) {
                QGraphicsView* view = views().first();
                QRect rect = QRect(mRubberBandStart.toPoint(), view->mapFromScene(e->scenePos()));

                mRubberBand->setGeometry(rect.normalized());
                mIsRubberband = true;
            }
        } else if (mMoving) {

            QGraphicsScene::mouseMoveEvent(e);
        }
    }
}

void Scene::mouseReleaseEvent(QGraphicsSceneMouseEvent* e)
{

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
            scaleModeMouseRelease(e);
            break;
        case Scene::RowEdit:
            rowEditMouseRelease(e);
            break;
        default:
            break;
    }

    if(mIsRubberband) {
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
    
    mHasSelection = false;
}

void Scene::colorModeMouseMove(QGraphicsSceneMouseEvent* e)
{
    if(e->buttons() != Qt::LeftButton)
        return;

    if(!mCurCell)
        return;

    mMoving = false;
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

    if(!mCurIndicator)
        return;

    mMoving = true;

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
        return;
    }

    if(!mCurIndicator && !mHasSelection && !mIsRubberband) {

        QPointF pt = e->buttonDownScenePos(Qt::LeftButton);
        //FIXME: dont hard code the offset for the indicator.
        pt = QPointF(pt.x() - 10, pt.y() - 10);

        undoStack()->push(new AddIndicator(this, pt));

    } else {
        if(mCurIndicator)
            mCurIndicator->setTextInteractionFlags(Qt::TextEditorInteraction);
    }

}

qreal Scene::scenePosToAngle(QPointF pt)
{

    qreal rads = atan2(pt.x(), pt.y());
    qreal angleX = rads * 180 / M_PI;

    return angleX;
}

void Scene::angleModeMousePress(QGraphicsSceneMouseEvent* e)
{
    Q_UNUSED(e);
    if(!mCurCell)
        return;

    mOldAngle = mCurCell->rotation();
    mPivotPt = QPointF(mCurCell->boundingRect().width()/2, mCurCell->boundingRect().bottom());
    mOrigin = mCurCell->mapToScene(mPivotPt);
}

void Scene::angleModeMouseMove(QGraphicsSceneMouseEvent* e)
{
    if(!mCurCell)
        return;

    mMoving = false;
    
    QPointF first = e->buttonDownScenePos(Qt::LeftButton);
    QPointF second = e->scenePos();

    QPointF rel1 = first - mOrigin;
    QPointF rel2 = second - mOrigin;
    qreal angle1 = scenePosToAngle(rel1);
    qreal angle2 = scenePosToAngle(rel2);

    mAngle = mOldAngle + (angle1 - angle2);

    qreal diff = fmod(mAngle, 45.0);
    qreal comp = abs(diff);
    if(comp < 4 /*&& !mSnapTo*/) {
        qreal div = mAngle - diff;
        mAngle = div;
/*FIXME: figure out a way to allow moving back with out snapping.
        mSnapTo = true;
    } else if(comp >= 4 && mSnapTo) {
        mSnapTo = false;
*/
    }
    
    qNormalizeAngle(mAngle);
    mCurCell->setTransformOriginPoint(mPivotPt);
    mCurCell->setRotation(mAngle);

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

void Scene::scaleModeMousePress(QGraphicsSceneMouseEvent* e)
{
    Q_UNUSED(e);
    if(!mCurCell)
        return;
    mOldScale = mCurCell->scale();
    mPrevScale = mOldScale;
    mPivotPt = QPointF(mCurCell->stitch()->width()/2, mCurCell->stitch()->height());
}

void Scene::scaleModeMouseMove(QGraphicsSceneMouseEvent* e)
{
    Q_UNUSED(e);
    if(!mCurCell)
        return;

    mMoving = false;
    
    QPointF delta = e->scenePos() - e->buttonDownScenePos(Qt::LeftButton);

    QSize newSize = QSize(mCurCell->boundingRect().width() + delta.x(), mCurCell->boundingRect().height() + delta.y());

    if((newSize.width() < 1 && newSize.width() > -1) || (newSize.height() < 1 && newSize.height() > -1))
        return;

    QPointF baseScale = QPointF(newSize.width() / mCurCell->boundingRect().width(),
                               newSize.height() / mCurCell->boundingRect().height());

    if(e->modifiers() == Qt::ControlModifier) {
        baseScale.ry() = baseScale.rx();
    }

    QPointF newScale = QPointF(baseScale.x() / mPrevScale.x(), baseScale.y() / mPrevScale.y());

    mCurCell->setScale(newScale.x(), newScale.y());
    mPrevScale = baseScale;
}

void Scene::scaleModeMouseRelease(QGraphicsSceneMouseEvent* e)
{
    
    Q_UNUSED(e);
    if(!mCurCell)
        return;

    undoStack()->push(new SetCellScale(this, mCurCell, mOldScale, mPivotPt));

    mOldScale.setX(1.0);
    mOldScale.setY(1.0);
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

        mRowLine = addLine(QLineF(e->scenePos(), e->scenePos()));
        mRowLine->setPen(QPen(QColor(Qt::red), 2));
        
    }
    mPreviousCell = mStartCell;

}

void Scene::rowEditMouseMove(QGraphicsSceneMouseEvent* e)
{
    
    if(!e->buttons() == Qt::LeftButton)
        return;

    mMoving = false;

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
                QGraphicsLineItem* line = addLine(QLineF(mPreviousCell->scenePos(), c->scenePos()));
                line->setPen(QPen(QColor(Qt::black), 2));
                mRowLines.append(line);

                mPreviousCell = c;
                startPt = c->scenePos();
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
    Q_UNUSED(e);

    mStartCell = 0;
    
    if(mRowLine) {
        removeItem(mRowLine);
        delete mRowLine;
        mRowLine = 0;
    }
    
}

void Scene::stitchModeMousePress(QGraphicsSceneMouseEvent* e)
{
    Q_UNUSED(e);
    
}

void Scene::stitchModeMouseMove(QGraphicsSceneMouseEvent* e)
{
    Q_UNUSED(e);

    if(!mCurCell)
        return;
    
    mMoving = true;
}

void Scene::stitchModeMouseRelease(QGraphicsSceneMouseEvent* e)
{
    //FIXME: foreach(stitch in selection()) create an undo group event.
    if(mCurCell && e->modifiers() != Qt::ControlModifier) {

        if(mCurCell->name() != mEditStitch && !mMoving)
            undoStack()->push(new SetCellStitch(this, mCurCell, mEditStitch));

        mCurCell = 0;

    } else if(!mIsRubberband && !mMoving && !mHasSelection) {

        if(e->button() == Qt::LeftButton && !(e->modifiers() & Qt::ControlModifier)) {

            AddCell* addCell = new AddCell(this, e->scenePos());
            undoStack()->push(addCell);
            addCell->cell()->setStitch(mEditStitch);

        }
    }
}

void Scene::createRow()
{
    
    if(selectedItems().count() <= 0)
        return;
    
    QList<CrochetCell*> r;

    foreach(QGraphicsItem* i, mRowSelection) {
        CrochetCell* c = qgraphicsitem_cast<CrochetCell*>(i);
        removeFromRows(c);
        c->useAlternateRenderer((grid.count() % 2));
        r.append(c);
    }
    grid.append(r);

}

void Scene::updateRow(int row)
{
    //FIXME: this overlaps the createRow code.
    if(selectedItems().count() <= 0)
        return;

    QList<CrochetCell*> r;

    foreach(QGraphicsItem* i, mRowSelection) {
        CrochetCell* c = qgraphicsitem_cast<CrochetCell*>(i);
        removeFromRows(c);
        c->useAlternateRenderer((row % 2));
        r.append(c);
    }

    grid.insert(row, r);
    
}

QPoint Scene::indexOf(CrochetCell* c)
{
    for(int y = 0; y < grid.count(); ++y) {
        if(grid[y].contains(c)) {
            return QPoint(grid[y].indexOf(c), y);
        }
    }

    return QPoint(-1,-1);
}

void Scene::highlightRow(int row)
{

    if(row >= grid.count())
        return;

    clearSelection();
    mRowSelection.clear();

    for(int i = 0; i < grid[row].count(); ++i) {
        CrochetCell* c = grid[row][i];
        if(c) {
            c->setSelected(true);
            mRowSelection.append(c);
        }
    }

    emit selectionChanged();
}

void Scene::moveRowDown(int row)
{
    QList<CrochetCell*> r = grid.takeAt(row);
    grid.insert(row + 1, r);
    updateStitchRenderer();
}

void Scene::moveRowUp(int row)
{
    QList<CrochetCell*> r = grid.takeAt(row);
    grid.insert(row - 1, r);
    updateStitchRenderer();
    
}

void Scene::removeRow(int row)
{

    grid.takeAt(row);
    updateStitchRenderer();

}

void Scene::updateStitchRenderer()
{

    for(int i = 0; i < grid.count(); ++i) {
        foreach(CrochetCell* c, grid[i]) {
            c->useAlternateRenderer((i % 2));
        }
    }

}

void Scene::drawRowLines(int row)
{
    if(grid.count() <= row)
        return;

    hideRowLines();

    QPointF start, end;

    int count = grid[row].count();

    QGraphicsItem* prev = grid[row].first();

    for(int i = 0; i < count; ++i) {
        QGraphicsItem* c = grid[row][i];
        if(!c)
            continue;

        if(prev != c) {
            start = prev->scenePos();
            end = c->scenePos();

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

//FIXME: simplify the number of foreach loops?
void Scene::alignSelection(int alignmentStyle)
{
    if(selectedItems().count() <= 0)
        return;

    QApplication::setOverrideCursor(Qt::WaitCursor);

    //left
    if(alignmentStyle == 1) {
        align(0,1);
        
    //center v
    } else if(alignmentStyle == 2) {
        align(0, 2);
        
    //right
    } else if(alignmentStyle == 3) {
        align(0, 3);
        
    //top
    } else if(alignmentStyle == 4) {
        align(1, 0);

    //center h
    } else if(alignmentStyle == 5) {
        align(2, 0);

    //bottom
    } else if(alignmentStyle == 6) {
        align(3, 0);

    //to path
    } else if(alignmentStyle == 7) {
        alignToPath();

    }

    QApplication::restoreOverrideCursor();
}

void Scene::distributeSelection(int distributionStyle)
{

    if(selectedItems().count() <= 0)
        return;
    
    QApplication::setOverrideCursor(Qt::WaitCursor);
    
    //left
    if(distributionStyle == 1) {
        distribute(0, 1);

    //center v
    } else if(distributionStyle == 2) {
        distribute(0, 2);

    //right
    } else if(distributionStyle == 3) {
        distribute(0, 3);

    //top
    } else if(distributionStyle == 4) {
        distribute(1, 0);

    //center h
    } else if(distributionStyle == 5) {
        distribute(2, 0);
        
    //bottom
    } else if(distributionStyle == 6) {
        distribute(3, 0);

    //to path
    } else if(distributionStyle == 7) {
        distributeToPath();

    }

    QApplication::restoreOverrideCursor();
}

void Scene::align(int vertical, int horizontal)
{

    qreal left = sceneRect().right();
    qreal right = sceneRect().left();
    qreal top = sceneRect().bottom();
    qreal bottom = sceneRect().top();
    
    foreach(QGraphicsItem* i, selectedItems()) {
        if(i->scenePos().x() < left)
            left = i->scenePos().x();
        if(i->sceneBoundingRect().right() > right)
            right = i->sceneBoundingRect().right();
        if(i->scenePos().y() < top)
            top = i->scenePos().y();
        if(i->sceneBoundingRect().bottom() > bottom)
            bottom = i->sceneBoundingRect().bottom();
    }

    qreal diff = right - left;
    qreal centerH = left + (diff / 2);
    diff = bottom - top;
    qreal centerV = top + (diff / 2);
    
    qreal baseX = 0;
    qreal baseY = 0;

    if(horizontal == 1)
        baseX = left;
    else if(horizontal == 2)
        baseX = centerH;
    else if(horizontal == 3)
        baseX = right;

    if(vertical == 1)
        baseY = top;
    else if(vertical == 2)
        baseY = centerV;
    else if(vertical == 3)
        baseY = bottom;
    
    undoStack()->beginMacro("align selection");
    foreach(QGraphicsItem* i, selectedItems()) {
        QPointF oldPos = i->scenePos();
        qreal newX = baseX;
        qreal newY = baseY;
        
        if(horizontal == 0) {
            newX = i->scenePos().x();
        } else if(horizontal == 2) {
            newX -= (i->sceneBoundingRect().width()/2);
        } else if(horizontal == 3) {
            newX -= i->sceneBoundingRect().width();
        }

        if(vertical == 0) {
            newY = i->scenePos().y();
        } else if(vertical == 2) {
            newY -= (i->sceneBoundingRect().height()/2);
        } else if(vertical == 3) {
            newY -= i->sceneBoundingRect().height();
        }
        
        i->setPos(newX, newY);
        undoStack()->push(new SetItemCoordinates(this, i, oldPos));
    }
    undoStack()->endMacro();
    
}

void Scene::distribute(int vertical, int horizontal)
{
    QList<QGraphicsItem*> unsorted = selectedItems();
    QList<QGraphicsItem*> sortedH;
    QList<QGraphicsItem*> sortedV;
    
    qreal left = sceneRect().right();
    qreal right = sceneRect().left();
    qreal top = sceneRect().bottom();
    qreal bottom = sceneRect().top();

    foreach(QGraphicsItem* i, unsorted) {
        
        if(horizontal != 0) {
            qreal width = 0; //left, center

            if(horizontal == 3)
                width = i->boundingRect().width(); //right

            qreal ptX = i->scenePos().x() + width;
            if(ptX > right)
                right = ptX;
            if(ptX < left)
                left = ptX;

            if(sortedH.count() <= 0) {
                sortedH.append(i);
            } else {
                bool added = false;
                for(int s = 0; s < sortedH.count(); ++s) {
                    qreal curX = sortedH[s]->scenePos().x() + sortedH[s]->boundingRect().width();
                    if(ptX < curX) {
                        sortedH.insert(s, i);
                        added = true;
                        break;
                    }
                }

                if(!added)
                    sortedH.append(i);
            }
        }

        if(vertical != 0) {
            qreal height = 0; //left, center

            if(vertical == 3)
                height = i->boundingRect().height(); //right

            qreal ptY = i->scenePos().y() + height;
            if(ptY < top)
                top = ptY;
            if(ptY > bottom)
                bottom = ptY;

            if(sortedV.count() <= 0) {
                sortedV.append(i);
            } else {
                bool added = false;
                for(int s = 0; s < sortedV.count(); ++s) {
                    qreal curY = sortedV[s]->scenePos().y() + sortedV[s]->boundingRect().height();
                    if(ptY < curY) {
                        sortedV.insert(s, i);
                        added = true;
                        break;
                    }
                }

                if(!added)
                    sortedV.append(i);
            }
        }
    }
    if(horizontal == 2) {
        //get the centers of the cells.
        left += (sortedH.first()->boundingRect().width() / 2);
        right += (sortedH.last()->boundingRect().width() / 2);
    }
    if(vertical == 2) {
        //get the centers of the cells.
        top += (sortedV.first()->boundingRect().height() / 2);
        bottom += (sortedV.last()->boundingRect().height() / 2);
    }

    qreal diff = right - left;
    qreal spaceH = diff / (sortedH.count() - 1);
    diff = bottom - top;
    qreal spaceV = diff / (sortedV.count() - 1);

    undoStack()->beginMacro("distribute selection");

    foreach(QGraphicsItem* i, unsorted) {
        qreal adjustH = 0; //left
        qreal adjustV = 0; //top

        if(horizontal == 2)
            adjustH = i->boundingRect().width() / 2; //center h
        else if(horizontal == 3)
            adjustH = i->boundingRect().width(); //right
            
        if(vertical == 2)
            adjustV = i->boundingRect().height() / 2; //center v
        else if(vertical == 3)
            adjustV = i->boundingRect().height(); //bottom

        int idxX = sortedH.indexOf(i);
        int idxY = sortedV.indexOf(i);

        qreal newX = 0;
        qreal newY = 0;
        
        if(horizontal == 0)
            newX = i->scenePos().x();
        else
            newX = left - adjustH + (idxX * spaceH);

        if(vertical == 0)
            newY = i->scenePos().y();
        else
            newY = top - adjustV + (idxY * spaceV);
        
        QPointF oldPos = i->scenePos();
        i->setPos(newX, newY);
        undoStack()->push(new SetItemCoordinates(this, i, oldPos));
    }
    undoStack()->endMacro();
}

void Scene::alignToPath()
{

}

void Scene::distributeToPath()
{

}

void Scene::createRowsChart(int rows, int cols, QString defStitch, QSizeF rowSize)
{
    
    mDefaultSize = rowSize;
    mDefaultStitch = defStitch;
    arrangeGrid(QSize(rows, cols), QSize(1, 1), rowSize.toSize(), false);
    
}

void Scene::arrangeGrid(QSize grd, QSize alignment, QSize spacing, bool useSelection)
{
    Q_UNUSED(alignment);
    if(useSelection) {
    /*
        boundingRect().height()[.width()] / rows/[cols] = MOE for spacing.

        look in each "box" for a stitch
        if found position stitch per user rules above.
        else leave box empty?

    */
    } else {
        //create new cells.
        //TODO: figure out how to deal with spacing.

        for(int x = grd.width(); x > 0; --x) {

            QList<CrochetCell*> r;
            for(int y = grd.height(); y > 0; --y) {
                CrochetCell* c = new CrochetCell();
                //FIXME: use the user selected stitch
                c->setStitch(mDefaultStitch);
                addItem(c);
                r.append(c);
                
                c->useAlternateRenderer(((grd.width() - x) % 2));
                c->setPos((c->stitch()->width() + spacing.width()) * y, spacing.height() * x);
                c->setToolTip(QString("Row: %1, Stitch: %2").arg(grd.width() - x + 1).arg(r.indexOf(c) + 1));
            }

            grid.insert(0, r);
        }
    }
}

void Scene::mirror(int direction)
{
    if(selectedItems().count() <= 0)
        return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    QRectF rect = selectedItemsBoundingRect(selectedItems());

    QList<QGraphicsItem*> list = selectedItems();

    clearSelection();

    undoStack()->beginMacro("mirror selection");
    if(direction == 1) { //left

        foreach(QGraphicsItem* item, list) {
            if(item->type() == CrochetCell::Type) {
                CrochetCell* c = qgraphicsitem_cast<CrochetCell*>(item);
                QPointF oldPos = item->pos();

                AddCell* addCellCmd = new AddCell(this, oldPos);
                undoStack()->push(addCellCmd);
                CrochetCell* copy = c->copy(addCellCmd->cell());
                
                qreal diff = item->pos().x() - rect.left();
                copy->setPos(rect.left() - diff, item->pos().y());

                qreal newAngle = 360 - copy->rotation();
                qNormalizeAngle(newAngle);
                copy->setRotation(newAngle);
                copy->setSelected(true);

            }
        }

    } else if(direction == 2) { //right

        foreach(QGraphicsItem* item, list) {
            if(item->type() == CrochetCell::Type) {
                CrochetCell* c = qgraphicsitem_cast<CrochetCell*>(item);
                QPointF oldPos = item->pos();

                AddCell* addCellCmd = new AddCell(this, oldPos);
                undoStack()->push(addCellCmd);
                CrochetCell* copy = c->copy(addCellCmd->cell());
                
                qreal diff = rect.right() - item->pos().x();
                copy->setPos(rect.right() + diff, item->pos().y());

                qreal newAngle = 360 - copy->rotation();
                qNormalizeAngle(newAngle);
                copy->setRotation(newAngle);
                copy->setSelected(true);
            }
        }

    } else if(direction == 3) { //up

        foreach(QGraphicsItem* item, list) {
            if(item->type() == CrochetCell::Type) {
                CrochetCell* c = qgraphicsitem_cast<CrochetCell*>(item);
                QPointF oldPos = item->pos();

                AddCell* addCellCmd = new AddCell(this, oldPos);
                undoStack()->push(addCellCmd);
                CrochetCell* copy = c->copy(addCellCmd->cell());

                qreal diff = item->pos().y() - rect.top();
                copy->setPos(item->pos().x(), rect.top() - diff - (2*item->sceneBoundingRect().height()));
                undoStack()->push(new SetItemCoordinates(this, copy, oldPos));

                qreal newAngle = 360 - copy->rotation() + 180;
                qNormalizeAngle(newAngle);
                copy->setTransformOriginPoint(copy->boundingRect().width()/2, 0);
                copy->setRotation(newAngle);
                copy->setSelected(true);
            }
        }

    } else if(direction == 4) { //down

        foreach(QGraphicsItem* item, list) {
            if(item->type() == CrochetCell::Type) {
                CrochetCell* c = qgraphicsitem_cast<CrochetCell*>(item);
                QPointF oldPos = item->pos();
                
                AddCell* addCellCmd = new AddCell(this, oldPos);
                undoStack()->push(addCellCmd);
                CrochetCell* copy = c->copy(addCellCmd->cell());
                
                qreal diff = rect.bottom() - item->pos().y();
                copy->setPos(item->pos().x(), rect.bottom() - diff);
                undoStack()->push(new SetItemCoordinates(this, copy, oldPos));

                qreal newAngle = 360 - copy->rotation() + 180;
                qNormalizeAngle(newAngle);
                copy->setRotation(newAngle);
                copy->setSelected(true);
            }
        }
    }
    undoStack()->endMacro();

    QApplication::restoreOverrideCursor();
}

void Scene::rotate(qreal degrees)
{
    if(selectedItems().count() <= 0)
        return;

    qreal newAngle = degrees;
    qNormalizeAngle(newAngle);

    QPointF pivotPt;
    
    if(mCenterSymbol) {
        pivotPt = mCenterSymbol->boundingRect().center();
    } else {
        pivotPt = selectedItemsBoundingRect(selectedItems()).bottomLeft();
    }
    
    QGraphicsItemGroup* g = createItemGroup(selectedItems());
    g->setTransformOriginPoint(pivotPt);
    g->setRotation(newAngle);
    destroyItemGroup(g);


    //undoStack()->push(new SetItemRotation(this, selectedItems(), degrees));
}

void Scene::copy()
{
    if(selectedItems().count() <= 0)
        return;

    QByteArray copyData;
    QDataStream stream(&copyData, QIODevice::WriteOnly);
    QMimeData *mimeData = new QMimeData;

    stream << selectedItems().count();
    copyRecursively(stream, selectedItems());

    mimeData->setData("application/crochet-cells", copyData);
    QApplication::clipboard()->setMimeData(mimeData);

}

void Scene::copyRecursively(QDataStream &stream, QList<QGraphicsItem*> items)
{
    foreach(QGraphicsItem* item, items) {
        switch(item->type()) {
            case CrochetCell::Type: {
                CrochetCell* c = qgraphicsitem_cast<CrochetCell*>(item);
                stream << c->type() << c->name() << c->color()
                    << c->rotation() << c->scale() << c->transformOriginPoint() << c->pos();
                break;
            }
            case Indicator::Type: {
                Indicator* i = qgraphicsitem_cast<Indicator*>(item);
                stream << i->type() << i->scenePos() << i->text();
                break;
            }
            case QGraphicsItemGroup::Type: {
                QGraphicsItemGroup* group = qgraphicsitem_cast<QGraphicsItemGroup*>(item);
                stream << group->type() << group->childItems().count();
                copyRecursively(stream, group->childItems());
                break;
            }
            default:
                qWarning() << "Copy: Unknown data type.";
                break;
        }
    }
}
    
void Scene::paste()
{
    const QMimeData *mimeData = QApplication::clipboard()->mimeData();

    QByteArray pasteData = mimeData->data("application/crochet-cells");
    QDataStream stream(&pasteData, QIODevice::ReadOnly);

    clearSelection();

    undoStack()->beginMacro("paste items");
    int count = 0;
    stream >> count;
    QList<QGraphicsItem*> items;
    for(int i = 0; i < count; ++i) {
        pasteRecursively(stream, &items);
    }

    foreach(QGraphicsItem* item, items)
        item->setSelected(true);
    
    undoStack()->endMacro();
}


void Scene::pasteRecursively(QDataStream &stream, QList<QGraphicsItem*> *group)
{
    int type;
    stream >> type;
    switch(type) {

        case CrochetCell::Type: {
            QString name;
            QColor color;
            qreal angle, scale;
            QPointF pos, transPoint;

            stream >> name >> color >> angle >> scale >> transPoint >> pos;

            AddCell* addCmd = new AddCell(this, pos);
            undoStack()->push(addCmd);
            CrochetCell* c = addCmd->cell();

            c->setStitch(name);
            c->setColor(color);

            qDebug() << "FIXME: paste set scale";
            c->setScale(scale, scale);
            c->setTransformOriginPoint(transPoint);
            c->setRotation(angle);

            c->setSelected(false);
            group->append(c);
            break;
        }
        case Indicator::Type: {
            Indicator* i = new Indicator();
            //FIXME: add this indicator to the undo stack.
            QPointF pos;
            QString text;

            stream >> pos >> text;

            i->setText(text);
            addItem(i);
            i->setPos(pos);
            
            i->setSelected(false);
            group->append(i);
            break;
        }
        case QGraphicsItemGroup::Type: {
            int childCount;
            stream >> childCount;
            QList<QGraphicsItem*> items;
            for(int i = 0; i < childCount; ++i) {
                pasteRecursively(stream, &items);
            }

            GroupItems* grpItems = new GroupItems(this, items);
            undoStack()->push(grpItems);
            QGraphicsItemGroup* g = grpItems->group();

            g->setSelected(false);
            group->append(g);
            break;
        }
        default: {
            qWarning() << "Paste: Unknown data type.";
            break;
        }
    }

}

void Scene::cut()
{
    if(selectedItems().count() <= 0)
        return;

    copy();

    undoStack()->beginMacro(tr("cut items"));
    foreach(QGraphicsItem* item, selectedItems()) {

        switch(item->type()) {
            case CrochetCell::Type: {
                CrochetCell* c = qgraphicsitem_cast<CrochetCell*>(item);
                undoStack()->push(new RemoveCell(this, c));
                break;
            }
            case Indicator::Type: {
            Indicator* i = qgraphicsitem_cast<Indicator*>(item);
                undoStack()->push(new RemoveIndicator(this, i));
                break;
            }
            default:
                break;
        }
    }
    undoStack()->endMacro();

}

QRectF Scene::selectedItemsBoundingRect(QList<QGraphicsItem*> items)
{
    if(items.count() <= 0)
        return QRectF();

    qreal left = sceneRect().right();
    qreal right = sceneRect().left();
    qreal top = sceneRect().bottom();
    qreal bottom = sceneRect().top();

    //height and width of the object at the extremes.
    qreal leftW = 0;

    foreach(QGraphicsItem* i, items) {
        if(i->scenePos().x() < left) {
            left = i->scenePos().x();
            leftW = i->sceneBoundingRect().width() - i->boundingRect().width();
        }
        if(i->scenePos().x() + i->boundingRect().right() > right) {
            right = i->scenePos().x() + i->boundingRect().right();
        }
        if(i->scenePos().y() < top) {
            top = i->scenePos().y();
        }
        if(i->scenePos().y() + i->boundingRect().bottom() > bottom) {
            bottom = i->scenePos().y() + i->boundingRect().bottom();
        }
    }

    return QRectF(QPointF(left - leftW, top), QPointF(right, bottom));
}

void Scene::group()
{
    if(selectedItems().count() <= 0)
        return;

    undoStack()->push(new GroupItems(this, selectedItems()));
}

QGraphicsItemGroup* Scene::group(QList<QGraphicsItem*> items, QGraphicsItemGroup* g)
{

    //clear selection because we're going to create a new selection.
    clearSelection();

    if(!g) {
        g = createItemGroup(items);
    } else {
        foreach(QGraphicsItem* i, items) {
            i->setSelected(false);
            g->addToGroup(i);
        }
    }

    g->setFlag(QGraphicsItem::ItemIsMovable);
    g->setFlag(QGraphicsItem::ItemIsSelectable);
    mGroups.append(g);

    g->setSelected(true);

    return g;
}

void Scene::ungroup()
{
    if(selectedItems().count() <= 0)
        return;

    foreach(QGraphicsItem* item, selectedItems()) {
        if(item->type() == QGraphicsItemGroup::Type) {
            QGraphicsItemGroup* group = qgraphicsitem_cast<QGraphicsItemGroup*>(item);
            undoStack()->push(new UngroupItems(this, group));
        }
    }
}

void Scene::ungroup(QGraphicsItemGroup* group)
{

    mGroups.removeOne(group);
    foreach(QGraphicsItem* item, group->childItems()) {
        group->removeFromGroup(item);
    }

}

/**********
 *Rounds Specific functions:
 */

void Scene::setShowChartCenter(bool state)
{
    mShowChartCenter = state;

    if(mShowChartCenter) {
        if(!mCenterSymbol) {
            QPen pen;
            pen.setWidth(5);

            double radius = (defaultSize().height() * 0.45);

            QGraphicsView* view =  views().first();
            QPointF topLeft = view->mapToScene(0, 0);
            QPointF bottomRight = view->mapToScene(view->width(), view->height());
            
            QRectF rect(topLeft, bottomRight);
            
            mCenterSymbol = addEllipse(rect.center().x()-radius, rect.center().y()-radius, radius * 2, radius * 2, pen);
            mCenterSymbol->setToolTip(tr("Chart Center"));
            mCenterSymbol->setFlag(QGraphicsItem::ItemIsMovable);
            mCenterSymbol->setFlag(QGraphicsItem::ItemIsSelectable);
        } else {
            addItem(mCenterSymbol);
        }
    } else {
        removeItem(mCenterSymbol);
    }

}

void Scene::createRoundsChart(int rows, int cols, QString stitch, QSizeF rowSize)
{

    defaultSize() = rowSize;

    for(int i = 0; i < rows; ++i) {
        //FIXME: this padding should be dependant on the height of the sts.
        int pad = i * 12;

        createRow(i, cols + pad, stitch);
    }

    setShowChartCenter(Settings::inst()->value("showChartCenter").toBool());

    initDemoBackground();
}

void Scene::setCellPosition(int row, int column, CrochetCell* c, int columns)
{
    double widthInDegrees = 360.0 / columns;

    double radius = defaultSize().height() * (row + 1) + (defaultSize().height() * 0.5);

    double degrees = widthInDegrees * column;
    QPointF finish = calcPoint(radius, degrees, QPointF(0,0));

    qreal delta = defaultSize().width() * 0.5;
    c->setPos(finish.x() - delta, finish.y());
    c->setTransform(QTransform().translate(delta,0).rotate(degrees + 90).translate(-delta, 0));
    c->setRotation(degrees + 90);
    c->setToolTip(tr("Row: %1, St: %2").arg(row+1).arg(column+1));
}

QPointF Scene::calcPoint(double radius, double angleInDegrees, QPointF origin)
{
    // Convert from degrees to radians via multiplication by PI/180
    qreal x = (radius * cos(angleInDegrees * M_PI / 180)) + origin.x();
    qreal y = (radius * sin(angleInDegrees * M_PI / 180)) + origin.y();
    return QPointF(x, y);
}

void Scene::createRow(int row, int columns, QString stitch)
{
    CrochetCell* c = 0;

    QList<CrochetCell*> modelRow;
    for(int i = 0; i < columns; ++i) {
        c = new CrochetCell();
        c->setStitch(stitch, (row % 2));
        addItem(c);
        
        modelRow.append(c);
        setCellPosition(row, i, c, columns);
    }
    grid.insert(row, modelRow);
}
