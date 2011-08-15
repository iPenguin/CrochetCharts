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
    mRowSpacing(9),
    mStyle(Scene::Rows),
    mMode(Scene::StitchMode),
    mCenterSymbol(0),
    mEditStitch("ch"),
    mEditFgColor(QColor(Qt::black)),
    mEditBgColor(QColor(Qt::white)),
    mDefaultSize(QSizeF(32.0, 96.0))
{
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

void Scene::setShowChartCenter(bool state)
{
    mShowChartCenter = state;

    if(mStyle == Scene::Rounds) {
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

void Scene::addIndicator(Indicator* i)
{
    addItem(i);
    mIndicators.append(i);
}

void Scene::removeIndicator(Indicator *i)
{
    removeItem(i);
    mIndicators.removeOne(i);
    update();
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

void Scene::removeCell(CrochetCell *c)
{
    removeItem(c);
    for(int i = 0; i < mGrid.count(); ++i) {
        if (mGrid[i].contains(c)) {
            mGrid[i].removeOne(c);
        }
    }

}

int Scene::rowCount()
{
    return mGrid.count();
}

int Scene::columnCount(int row)
{
    if(mGrid.count() <= row)
        return 0;
    return mGrid[row].count();
}

void Scene::appendCell(int row, CrochetCell *c, bool fromSave)
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
        if(mStyle == Scene::Rounds)
            redistributeCells(row);
    }
}

void Scene::addCell(QPoint p, CrochetCell* c)
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

void Scene::setCellPosition(int row, int column, CrochetCell *c, int columns, bool updateAnchor)
{
    if(mStyle == Scene::Rounds) {
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

void Scene::redistributeCells(int row)
{
    if(row >= mGrid.count())
        return;
    int columns = mGrid[row].count();

    for(int i = 0; i < columns; ++i) {
        CrochetCell *c = mGrid[row].at(i);
        setCellPosition(row, i, c, columns, true);
    }
}

void Scene::createChart(Scene::ChartStyle style, int rows, int cols, QString stitch, QSizeF rowSize)
{
    mStyle = style;
    if(mStyle == Scene::Blank) {


    } else {
        mDefaultSize = rowSize;

        for(int i = 0; i < rows; ++i) {
            int pad = 0;
            if(mStyle == Scene::Rounds)
                pad = i*8;

            createRow(i, cols + pad, stitch);
        }

        setShowChartCenter(Settings::inst()->value("showChartCenter").toBool());
    }

    initDemoBackground();
}

void Scene::createRow(int row, int columns, QString stitch)
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

int Scene::getClosestRow(QPointF mousePosition)
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

int Scene::getClosestColumn(QPointF mousePosition, int row)
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

QPointF Scene::calcPoint(double radius, double angleInDegrees, QPointF origin)
{
    // Convert from degrees to radians via multiplication by PI/180
    qreal x = (radius * cos(angleInDegrees * M_PI / 180)) + origin.x();
    qreal y = (radius * sin(angleInDegrees * M_PI / 180)) + origin.y();
    return QPointF(x, y);
}

void Scene::stitchUpdated(QString oldSt, QString newSt)
{
    Q_UNUSED(newSt);
    
    if(oldSt.isEmpty() || oldSt.isNull())
        return;
    
    CrochetCell *c = qobject_cast<CrochetCell*>(sender());
    if(!c)
        return;

    QPoint pos = findGridPosition(c);
    
}

void Scene::updateSelection(QPolygonF selection)
{
    QPainterPath path;
    path.addPolygon(selection);
    setSelectionArea(path);
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
    
    switch(mMode) {
        case Scene::StitchMode:
            stitchModeMousePress(e);
            break;
        case Scene::ColorMode:
            colorModeMousePress(e);
            break;
        case Scene::PositionMode:
            positionModeMousePress(e);
            break;
        case Scene::AngleMode:
            angleModeMousePress(e);
            break;
        case Scene::StretchMode:
            stretchModeMousePress(e);
            break;
        case Scene::IndicatorMode:
            indicatorModeMousePress(e);
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

void Scene::mouseMoveEvent(QGraphicsSceneMouseEvent *e)
{

    switch(mMode) {
        case Scene::StitchMode:
            stitchModeMouseMove(e);
            break;
        case Scene::ColorMode:
            colorModeMouseMove(e);
            break;
        case Scene::PositionMode:
            QGraphicsScene::mouseMoveEvent(e);
            break;
        case Scene::AngleMode:
            angleModeMouseMove(e);
            break;
        case Scene::StretchMode:
            stretchModeMouseMove(e);
            break;
        case Scene::IndicatorMode:
            indicatorModeMouseMove(e);
            QGraphicsScene::mouseMoveEvent(e);
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

    
    switch(mMode) {
        case Scene::StitchMode:
            stitchModeMouseRelease(e);
            break;
        case Scene::ColorMode:
            colorModeMouseRelease(e);
            break;
        case Scene::PositionMode:
            positionModeMouseRelease(e);
            break;
        case Scene::AngleMode:
            angleModeMouseRelease(e);
            break;
        case Scene::StretchMode:
            stretchModeMouseRelease(e);
            break;
        case Scene::IndicatorMode:
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

    QGraphicsScene::mouseReleaseEvent(e);
}

void Scene::colorModeMousePress(QGraphicsSceneMouseEvent* e)
{
    Q_UNUSED(e);
}

void Scene::colorModeMouseMove(QGraphicsSceneMouseEvent* e)
{
    if(e->buttons() != Qt::LeftButton)
        return;
    
    if(!mCurCell)
        return;
    
    if(mCurCell->color() != mEditBgColor)
        mUndoStack.push(new SetCellColor(this, mCurCell, mEditBgColor));
}

void Scene::colorModeMouseRelease(QGraphicsSceneMouseEvent* e)
{
    Q_UNUSED(e);
    if(!mCurCell)
        return;

    if(mCurCell->color() != mEditBgColor)
        mUndoStack.push(new SetCellColor(this, mCurCell, mEditBgColor));
}

void Scene::positionModeMousePress(QGraphicsSceneMouseEvent* e)
{
    Q_UNUSED(e);
    if(selectedItems().count() <= 0)
        return;

    foreach(QGraphicsItem *item, selectedItems()) {
        mOldPositions.insert(item, item->pos());
    }

}

void Scene::positionModeMouseMove(QGraphicsSceneMouseEvent* e)
{
    Q_UNUSED(e);
}

void Scene::positionModeMouseRelease(QGraphicsSceneMouseEvent* e)
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

void Scene::stitchModeMousePress(QGraphicsSceneMouseEvent* e)
{
    Q_UNUSED(e);
}

void Scene::stitchModeMouseMove(QGraphicsSceneMouseEvent* e)
{
    if(e->buttons() != Qt::LeftButton)
        return;
    
    if(!mCurCell)
        return;
    
    if(mCurCell->name() != mEditStitch)
        mUndoStack.push(new SetCellStitch(this, mCurCell, mEditStitch));
}

void Scene::stitchModeMouseRelease(QGraphicsSceneMouseEvent* e)
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
        if(mStyle == Scene::Rounds) {
            y = getClosestRow(e->scenePos());
            //FIXME: the row has to be passed in because getClosestRow modifies the row
            x = getClosestColumn(e->scenePos(), y);
        } else if (mStyle == Scene::Rows) {
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

void Scene::angleModeMousePress(QGraphicsSceneMouseEvent *e)
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

void Scene::angleModeMouseMove(QGraphicsSceneMouseEvent *e)
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

void Scene::angleModeMouseRelease(QGraphicsSceneMouseEvent *e)
{
    Q_UNUSED(e);
    mCurCellRotation = 0;
}

void Scene::stretchModeMousePress(QGraphicsSceneMouseEvent* e)
{
    Q_UNUSED(e);
}

void Scene::stretchModeMouseMove(QGraphicsSceneMouseEvent* e)
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

void Scene::stretchModeMouseRelease(QGraphicsSceneMouseEvent* e)
{
    Q_UNUSED(e);
}

void Scene::indicatorModeMousePress(QGraphicsSceneMouseEvent *e)
{
    Q_UNUSED(e);
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

        //connect(i, SIGNAL(lostFocus(Indicator*)), this, SLOT(editorLostFocus(Indicator*)));
        //connect(i, SIGNAL(selectedChange(QGraphicsItem*)), this, SIGNAL(itemSelected(QGraphicsItem*)));
    } else {
        mCurIndicator->setTextInteractionFlags(Qt::TextEditorInteraction);
    }
    
}
