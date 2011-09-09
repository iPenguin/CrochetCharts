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

void Scene::createChart(int rows, int cols, QString stitch, QSizeF rowSize)
{
    Q_UNUSED(rows);
    Q_UNUSED(cols);
    Q_UNUSED(stitch);
    Q_UNUSED(rowSize);

}

void Scene::createRow(int row, int columns, QString stitch)
{
    Q_UNUSED(row);
    Q_UNUSED(columns);
    Q_UNUSED(stitch);

}

void Scene::removeCell(CrochetCell* c)
{
    removeItem(c);
    removeFromRows(c);
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
    if(keyEvent->key() == Qt::Key_Delete || keyEvent->key() == Qt::Key_Backspace) {
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
    if(selectedItems().count() > 0)
        mHasSelection = true;
    
    QGraphicsScene::mousePressEvent(e);
    
    mLeftButtonDownPos = e->buttonDownPos(Qt::LeftButton);

    QGraphicsItem* gi = itemAt(e->scenePos());
    if(gi) {
        
        switch(gi->type()) {
            case CrochetCell::Type: {
                CrochetCell* c = qgraphicsitem_cast<CrochetCell*>(gi);
                mCurCell = c;
                mCellStartPos = mCurCell->pos();
                mDiff = QSizeF(e->scenePos().x() - mCellStartPos.x(), e->scenePos().y() - mCellStartPos.y());
                break;
            }
            case Indicator::Type: {
                Indicator* i = qgraphicsitem_cast<Indicator*>(gi);
            
                mCurIndicator = i;
                mCellStartPos = i->pos();
                break;
            }
        }
    }

    mMoving = false;
    mIsRubberband = false;
    
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
    
    qreal diff = (e->buttonDownScenePos(Qt::LeftButton)- e->scenePos()).manhattanLength();

    if(diff >= QApplication::startDragDistance()) {

        if(!mCurCell) {
            if(mRubberBand) {
                QGraphicsView* view = views().first();
                QRect rect = QRect(mRubberBandStart.toPoint(), view->mapFromScene(e->scenePos()));

                mRubberBand->setGeometry(rect.normalized());
                mIsRubberband = true;
            }
        } else {
            mMoving = true;
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
            stretchModeMouseRelease(e);
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

    if(!mCurIndicator && !mHasSelection) {

        QPointF pt = e->buttonDownScenePos(Qt::LeftButton);
        //FIXME: dont hard code the offset for the indicator.
        pt = QPointF(pt.x() - 10, pt.y() - 10);

        undoStack()->push(new AddIndicator(this, pt));

    } else {
        if(mCurIndicator)
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

        mRowLine = addLine(QLineF(e->scenePos(), e->scenePos()));
        mRowLine->setPen(QPen(QColor(Qt::red), 2));
        
    }
    mPreviousCell = mStartCell;

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
    Q_UNUSED(e);

    mStartCell = 0;
    
    if(mRowLine) {
        removeItem(mRowLine);
        delete mRowLine;
        mRowLine = 0;
    }
    
}

void Scene::stitchModeMouseRelease(QGraphicsSceneMouseEvent* e)
{
    //FIXME: foreach(stitch in selection()) create an undo group event.
    if(mCurCell) {

        if(mCurCell->name() != mEditStitch && !mMoving)
            undoStack()->push(new SetCellStitch(this, mCurCell, mEditStitch));

        mCurCell = 0;

    } else if(!mIsRubberband && !mMoving && !mHasSelection){

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
            start = prev->pos();
            end = c->pos();

            QGraphicsLineItem *line = addLine(QLineF(start, end));
            line->setPen(QPen(QColor(Qt::red), 2));
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
        if(i->scenePos().x() > right)
            right = i->scenePos().x();
        if(i->scenePos().y() < top)
            top = i->scenePos().y();
        if(i->scenePos().y() > bottom)
            bottom = i->scenePos().y();
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
            newX -= (i->boundingRect().width()/2);
        }

        if(vertical == 0) {
            newY = i->scenePos().y();
        } else if(vertical == 2) {
            newY -= (i->boundingRect().height()/2);
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

    undoStack()->beginMacro("distribute selection");
    if(horizontal != 0) {
        if(horizontal == 2) {
            //get the centers of the cells.
            left += (sortedH.first()->boundingRect().width() / 2);
            right += (sortedH.last()->boundingRect().width() / 2);
        }

        qreal diff = right - left;
        qreal spaceH = diff / (sortedH.count() - 1);

        for(int i = 0; i < sortedH.count(); ++i) {
            qreal adjustH = 0; //left

            if(horizontal == 2)
                adjustH = sortedH[i]->boundingRect().width() / 2; //center h
            else if(horizontal == 3)
                adjustH = sortedH[i]->boundingRect().width(); //right

            QPointF oldPos = sortedH[i]->scenePos();
            sortedH[i]->setPos(left + (i * spaceH) - adjustH, sortedH[i]->scenePos().y());
            undoStack()->push(new SetItemCoordinates(this, sortedH[i], oldPos));
        }
    }

    if(vertical != 0) {
        if(vertical == 2) {
            //get the centers of the cells.
            top += (sortedV.first()->boundingRect().height() / 2);
            bottom += (sortedV.last()->boundingRect().height() / 2);
        }

        qreal diff = bottom - top;
        qreal spaceV = diff / (sortedV.count() - 1);


        for(int i = 0; i < sortedV.count(); ++i) {
            qreal adjustV = 0;

            if(vertical == 2)
                adjustV = sortedV[i]->boundingRect().height() / 2; //center v
            else if(vertical == 3)
                adjustV = sortedV[i]->boundingRect().height(); //bottom
            
            QPointF oldPos = sortedV[i]->scenePos();
            sortedV[i]->setPos(sortedV[i]->scenePos().x(), top + (i * spaceV) - adjustV);
            undoStack()->push(new SetItemCoordinates(this, sortedV[i], oldPos));
        }
    }

    undoStack()->endMacro();
}

void Scene::alignToPath()
{

}

void Scene::distributeToPath()
{

}

void Scene::arrangeGrid(QSize grid, QSize alignment, QSize spacing, bool useSelection)
{

    if(useSelection) {

        int rows = grid.width();
        int cols = grid.height();

        QRectF rect = selectionArea().boundingRect();
        rect.width() / rows;

/*

        if !useseleciton create cells and select them.

            distribute cells based on aligment.

        relPos = figure out relative position
        distribute based on positions.

        
*/      
/*
        qreal left = sceneRect().right();
        foreach(QGraphicsItem* i, selectedItems()) {
            if(i->scenePos().x() < left)
                left = i->scenePos().x();
        }

        undoStack()->beginMacro("align selection");
        foreach(QGraphicsItem* i, selectedItems()) {
                QPointF oldPos = i->scenePos();
                i->setPos(left, i->scenePos().y());
                undoStack()->push(new SetItemCoordinates(this, i, oldPos));
        }
        undoStack()->endMacro();


        qreal left = sceneRect().right();
        qreal right = sceneRect().left();

        foreach(QGraphicsItem* i, unsorted) {
            if(i->scenePos().x() > right)
                right = i->scenePos().x();
            if(i->scenePos().x() < left)
                left = i->scenePos().x();

            if(sorted.count() <= 0) {
                sorted.append(i);
            } else {
                bool added = false;
                for(int s = 0; s < sorted.count(); ++s) {
                    if(i->scenePos().x() < sorted[s]->scenePos().x()) {
                        sorted.insert(s, i);
                        added = true;
                        break;
                    }
                }

                if(!added)
                    sorted.append(i);
            }
        }

        qreal diff = right - left;
        qreal space = diff / (sorted.count() - 1);

        undoStack()->beginMacro("distribute selection");
        for(int i = 0; i < sorted.count(); ++i) {
            QPointF oldPos = sorted[i]->scenePos();
            sorted[i]->setPos(left + (i * space), sorted[i]->scenePos().y());
            undoStack()->push(new SetItemCoordinates(this, sorted[i], oldPos));
        }
        undoStack()->endMacro();

*/
    }
    
}

void Scene::copy()
{
    if(selectedItems().count() <= 0)
        return;

    QByteArray copyData;
    QDataStream stream(&copyData, QIODevice::WriteOnly);

    QMimeData *mimeData = new QMimeData;
    foreach(QGraphicsItem* item, selectedItems()) {
        switch(item->type()) {
            case CrochetCell::Type: {

                CrochetCell* c = qgraphicsitem_cast<CrochetCell*>(item);
                stream << c->type() << c->name() << c->color()
                       << c->angle() << c->scale() << c->transformOriginPoint() << c->scenePos();

                break;
            }
            case Indicator::Type: {
                Indicator* i = qgraphicsitem_cast<Indicator*>(item);
                stream << i->type() << i->scenePos() << i->text();
                break;
            }
            default:
                break;
        }
     }

     mimeData->setData("application/crochet-cells", copyData);
    QApplication::clipboard()->setMimeData(mimeData);

}

void Scene::paste()
{
    const QMimeData *mimeData = QApplication::clipboard()->mimeData();

    QByteArray pasteData = mimeData->data("application/crochet-cells");
    QDataStream stream(&pasteData, QIODevice::ReadOnly);

    int type;

    clearSelection();

    while(!stream.atEnd()) {

        stream >> type;
        if(type == CrochetCell::Type) {
            CrochetCell* c = new CrochetCell();
            QString name;
            QColor color;
            qreal angle, scale;
            QPointF pos, transPoint;

            stream >> name >> color >> angle >> scale >> transPoint >> pos;
            pos.rx() +=5;
            pos.ry() +=5;

            addCell(c);
            c->setStitch(name);
            c->setColor(color);
            c->setAngle(angle);
            c->setScale(scale, transPoint);
            c->setPos(pos);
            c->setSelected(true);

        } else if(type == Indicator::Type) {
            Indicator* i = new Indicator();

            QPointF pos;
            QString text;

            stream >> pos >> text;
            pos.rx() += 5;
            pos.ry() += 5;

            i->setText(text);
            addItem(i);
            i->setPos(pos);
            i->setSelected(true);

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
