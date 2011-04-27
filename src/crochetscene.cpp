/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "crochetscene.h"

#include "crochetcell.h"

#include <QFontMetrics>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsSceneEvent>

#include <math.h>

#include <QDebug>

#include "chartview.h"

#include "settings.h"
#include "stitchset.h"
#include "appinfo.h"
#include "crochetchartcommands.h"

CrochetScene::CrochetScene(QObject *parent)
    : QGraphicsScene(parent), mCurCell(0), mDiff(QSizeF(0,0)), mHighlightCell(0),
    mRubberBand(0), mRubberBandStart(QPointF(0,0)),
    mRowSpacing(8), mStyle(CrochetScene::Flat), mMode(CrochetScene::StitchMode),
    mFreeForm(false), mEditStitch("ch"),
    mEditFgColor(QColor(Qt::black)), mEditBgColor(QColor(Qt::white))
{
    mStitchWidth = 64;

    initDemoBackground();
}

CrochetScene::~CrochetScene()
{
    foreach(QGraphicsItem *i, items()) {
        delete i;
    }
    items().clear();
    mGrid.clear();
}

void CrochetScene::initDemoBackground()
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
                demoText->setBrush(QBrush(QColor(Qt::lightGray)));
                QPointF point = QPointF(rect.left() + c*stringWidth , rect.top() + i*(2*fontSize));
                demoText->setPos(point);
                demoText->setZValue(-1);
            }
        }

        //restore original rect. letting the demo text overflow off the scene.
        setSceneRect(rect);
    }
}

CrochetCell* CrochetScene::cell(int row, int column)
{
    Q_ASSERT(mGrid.count() > row);
    if(mGrid[row].count() <= column)
        return 0;

    return mGrid[row][column];
}

CrochetCell* CrochetScene::cell(QPoint position)
{
    return cell(position.y(), position.x());
}

void CrochetScene::removeCell(int row, int column)
{
    Q_ASSERT(mGrid.count() > row);
    if(mGrid[row].count() <= column)
        return;

    delete mGrid[row][column];
    mGrid[row].removeAt(column);
}

int CrochetScene::rowCount()
{
    return mGrid.count();
}

int CrochetScene::columnCount(int row)
{
    Q_ASSERT(mGrid.count() > row);
    return mGrid[row].count();
}

void CrochetScene::appendCell(int row, CrochetCell *c, bool fromSave)
{
    //append any missing rows.
    if(mGrid.count() <= row) {
        for(int i = mGrid.count(); i < row + 1; ++i) {
            QList<CrochetCell*> row;
            mGrid.append(row);
        }
    }
    
    addItem(c);
    mGrid[row].append(c);
    connect(c, SIGNAL(stitchChanged(QString,QString)), this, SIGNAL(stitchChanged(QString,QString)));
    connect(c, SIGNAL(colorChanged(QString,QString)), this, SIGNAL(colorChanged(QString,QString)));
    connect(c, SIGNAL(stitchChanged(QString,QString)), this, SLOT(stitchUpdated(QString,QString)));

    int col = mGrid[row].count() -1;
    setCellPosition(row, col, c, mGrid[row].count());
    c->setColor(QColor(Qt::white));
   
    if(fromSave)
        emit rowChanged(row);
    else {
        if(mStyle == CrochetScene::Round)
            redistributeCells(row);
    }
}

void CrochetScene::insertCell(QPoint p, CrochetCell* c)
{
    //TODO: simplify the connect() statements...
    addItem(c);
    mGrid[p.y()].insert(p.x(), c);
}

void CrochetScene::setCellPosition(int row, int column, CrochetCell *c, int columns, bool updateAnchor)
{
    if(mStyle == CrochetScene::Round) {
        double widthInDegrees = 360.0 / columns;
        int cols = (row == 0) ? columns : mGrid[0].count();
        qreal rowOneCirc = ((mRowSpacing + mStitchWidth) * cols);
        double circumference = (row*mRowSpacing) * mStitchWidth + rowOneCirc;
        double diameter = circumference / M_PI;
        double radius = diameter / 2;
        
        double degrees = widthInDegrees*column;
        QPointF finish = calcPoint(radius, degrees, QPointF(0,0));        
        c->setPos(finish.x() - 32, finish.y());
        if(updateAnchor || c->anchor().isNull())
            c->setAnchor(finish.x() - 32, finish.y());
        c->setTransform(QTransform().translate(32,0).rotate(degrees + 90).translate(-32, 0));
        c->setToolTip(QString::number(column+1));
        
    } else {
        c->setPos(column*64, row*64);
        if(updateAnchor || c->anchor().isNull())
            c->setAnchor(column*64, row*64);
        c->setToolTip(QString::number(column+1));
        c->setColor(QColor(Qt::white));
    }
}

void CrochetScene::redistributeCells(int row)
{
    if(row >= mGrid.count())
        return;
    int columns = mGrid[row].count();

    for(int i = 0; i < columns; ++i) {
        CrochetCell *c = mGrid[row].at(i);
        setCellPosition(row, i, c, columns, true);
    }
}

void CrochetScene::createChart(CrochetScene::ChartStyle style, int rows, int cols, QString stitch)
{
    mStyle = style;
    for(int i = 0; i < rows; ++i)
            createRow(i, cols + i*8, stitch);
    
    emit chartCreated(rows, cols);
}

void CrochetScene::createRow(int row, int columns, QString stitch)
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
    //emit rowChanged(row);
    emit rowAdded(row);
}

int CrochetScene::getClosestRow(QPointF mousePosition)
{
    qreal circumference = sqrt(mousePosition.x()*mousePosition.x() + mousePosition.y()*mousePosition.y()) * 2 * M_PI;

    qreal rowOneCirc = ((mRowSpacing + mStitchWidth) * mGrid[0].count());
    qreal temp = circumference - rowOneCirc;
    qreal temp2 = temp / mStitchWidth;
    //TODO: see if there is a way to finess the numbers here...?   
    int row = round(temp2 / mRowSpacing);
    if(row < 0)
        row = 0;
    return row;
}

int CrochetScene::getClosestColumn(QPointF mousePosition)
{
    int row = getClosestRow(mousePosition);
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

QPointF CrochetScene::calcPoint(double radius, double angleInDegrees, QPointF origin)
{
    // Convert from degrees to radians via multiplication by PI/180
    qreal x = (radius * cos(angleInDegrees * M_PI / 180)) + origin.x();
    qreal y = (radius * sin(angleInDegrees * M_PI / 180)) + origin.y();
    return QPointF(x, y);
}

void CrochetScene::stitchUpdated(QString oldSt, QString newSt)
{
    if(oldSt.isEmpty() || oldSt.isNull())
        return;
    
    CrochetCell *c = qobject_cast<CrochetCell*>(sender());
    if(!c)
        return;

    QPoint pos = findGridPosition(c);
    //FIXME: crash! 
    qDebug() << "FIXME:" << oldSt << newSt << pos;
    //emit rowChanged(pos.y());
    
}

void CrochetScene::updateSelection(QPolygonF selection)
{
    QPainterPath path;
    path.addPolygon(selection);
    setSelectionArea(path);
}

void CrochetScene::updateRubberBand(int dx, int dy) 
{

    if(mRubberBandStart.isNull())
        return;

    mRubberBandStart.setX(mRubberBandStart.x() + dx);
    mRubberBandStart.setY(mRubberBandStart.y() + dy);
}

QPoint CrochetScene::findGridPosition(CrochetCell* c)
{
    for(int y = 0; y < mGrid.count(); ++y) {
        if(mGrid[y].contains(c)) {
            return QPoint(mGrid[y].indexOf(c), y);
        }
    }
    
    return QPoint();
}

qreal CrochetScene::scenePosToAngle(QPointF pt)
{
    qreal tanx = pt.y() / pt.x();
    qreal rads = atan(tanx);
    qreal angleX = rads * 180 / M_PI;
    qreal angle = 0.0;
    if (pt.x() >= 0 && pt.y() >= 0)
        angle = angleX;
    else if(pt.x() <= 0 && pt.y() >= 0)
        angle = 180 + angleX;
    else if(pt.x() <= 0 && pt.y() <= 0)
        angle = 180 + angleX;
    else if(pt.x() >= 0 && pt.y() <= 0)
        angle = 360 + angleX;
    
    return angle;
}

void CrochetScene::mousePressEvent(QGraphicsSceneMouseEvent *e)
{
    QGraphicsScene::mousePressEvent(e);

    QGraphicsItem *gi = itemAt(e->scenePos());
    CrochetCell *c = qgraphicsitem_cast<CrochetCell*>(gi);
    if(c)
        mCurCell = c;
    
    switch(mMode) {
        case CrochetScene::StitchMode:
            stitchModeMousePress(e);
            break;
        case CrochetScene::ColorMode:
            colorModeMousePress(e);
            break;
        case CrochetScene::GridMode:
            gridModeMousePress(e);
            break;
        case CrochetScene::PositionMode:
            if(selectedItems().count() <= 0)
                positionModeMousePress(e);
            break;
        case CrochetScene::AngleMode:
            angleModeMousePress(e);
            break;
        case CrochetScene::StrechMode:
            strechModeMousePress(e);
            break;
        default:
            break;
    }
}

void CrochetScene::mouseMoveEvent(QGraphicsSceneMouseEvent *e)
{
    //highlightCell(e);

    switch(mMode) {
        case CrochetScene::StitchMode:
            stitchModeMouseMove(e);
            break;
        case CrochetScene::ColorMode:
            colorModeMouseMove(e);
            break;
        case CrochetScene::GridMode:
            gridModeMouseMove(e);
            break;
        case CrochetScene::PositionMode:
            QGraphicsScene::mouseMoveEvent(e);
            positionModeMouseMove(e);
            break;
        case CrochetScene::AngleMode:
            angleModeMouseMove(e);
            break;
        case CrochetScene::StrechMode:
            strechModeMouseMove(e);
            break;
        default:
            break;
    }
}

void CrochetScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *e)
{
    switch(mMode) {
        case CrochetScene::StitchMode:
            stitchModeMouseRelease(e);
            break;
        case CrochetScene::ColorMode:
            colorModeMouseRelease(e);
            break;
        case CrochetScene::GridMode:
            gridModeMouseRelease(e);
            break;
        case CrochetScene::PositionMode:
            positionModeMouseRelease(e);
            break;
        case CrochetScene::AngleMode:
            angleModeMouseRelease(e);
            break;
        case CrochetScene::StrechMode:
            strechModeMouseRelease(e);
            break;
        default:
            break;
    }

    if(mCurCell)
        mCurCell = 0;

    QGraphicsScene::mouseReleaseEvent(e);
}

void CrochetScene::highlightCell(QGraphicsSceneMouseEvent *e)
{
    QGraphicsItem *gi = itemAt(e->scenePos());
    CrochetCell *c = qgraphicsitem_cast<CrochetCell*>(gi);

    if(!c) {
        if(mHighlightCell)
            mHighlightCell->setHighlight(false);
        mHighlightCell = 0;
        return;
    }    

    if(mHighlightCell && mHighlightCell != c)
        mHighlightCell->setHighlight(false);
    
    mHighlightCell = c;
    mHighlightCell->setHighlight(true);
    
}

void CrochetScene::colorModeMousePress(QGraphicsSceneMouseEvent* e)
{
    Q_UNUSED(e);
}

void CrochetScene::colorModeMouseMove(QGraphicsSceneMouseEvent* e)
{
    if(e->buttons() != Qt::LeftButton)
        return;
    
    QGraphicsItem *gi = itemAt(e->scenePos());
    CrochetCell *c = qgraphicsitem_cast<CrochetCell*>(gi);
    if(!c)
        return;
    if(c->color() != mEditBgColor) {
        mUndoStack.push(new SetCellColor(this, findGridPosition(c), mEditBgColor));
    }
}

void CrochetScene::colorModeMouseRelease(QGraphicsSceneMouseEvent* e)
{
    Q_UNUSED(e);
    if(mCurCell) {
        if(mCurCell->color() != mEditBgColor)
            mUndoStack.push(new SetCellColor(this, findGridPosition(mCurCell), mEditBgColor));
    }
    
}

void CrochetScene::gridModeMousePress(QGraphicsSceneMouseEvent* e)
{
    //FIXME: combine getClosestRow & getClosestColumn into 1 function returning a QPoint.
    int y = getClosestRow(e->scenePos());   
    int x = getClosestColumn(e->scenePos());
    
    if(e->buttons() == Qt::LeftButton && !(e->modifiers() & Qt::ControlModifier)) {

        CrochetCell *c = new CrochetCell();
        c->setStitch(mEditStitch, (y % 2));
        c->setColor();
        insertCell(QPoint(x, y), c);
        setCellPosition(y, x, c, mGrid[y].count(), true);

    } else {
        if(!mCurCell)
            return;

        CrochetCell *c = mGrid[y].takeAt(x);
        removeItem(c);
        delete c;
        c = 0;
        mCurCell = 0;
        mHighlightCell = 0;
    }

    redistributeCells(y);
}

void CrochetScene::gridModeMouseMove(QGraphicsSceneMouseEvent* e)
{
    Q_UNUSED(e);
}

void CrochetScene::gridModeMouseRelease(QGraphicsSceneMouseEvent* e)
{
    Q_UNUSED(e);
}

void CrochetScene::positionModeMousePress(QGraphicsSceneMouseEvent* e)
{
    if(e->buttons() == Qt::RightButton && mStyle == CrochetScene::Round) {
        int row = getClosestRow(e->scenePos());
        redistributeCells(row);
    }
        
    if(e->buttons() != Qt::LeftButton)
        return;

    ChartView *view = qobject_cast<ChartView*>(parent());
    
    if(!mRubberBand)
        mRubberBand = new QRubberBand(QRubberBand::Rectangle, view);

    mRubberBandStart = view->mapFromScene(e->scenePos());

    mRubberBand->setGeometry(QRect(mRubberBandStart.toPoint(), QSize()));
    mRubberBand->show();
    
    if(mCurCell)
        mDiff = QSizeF(mCurCell->transform().dx(), mCurCell->transform().dy());
}

void CrochetScene::positionModeMouseMove(QGraphicsSceneMouseEvent* e)
{
    if(mRubberBand) {
        ChartView *view = qobject_cast<ChartView*>(parent());
        QRect rect = QRect(mRubberBandStart.toPoint(), view->mapFromScene(e->scenePos()));    
    
        mRubberBand->setGeometry(rect.normalized());
    }
}

void CrochetScene::positionModeMouseRelease(QGraphicsSceneMouseEvent* e)
{
    mDiff.setHeight(0);
    mDiff.setWidth(0);

    if(mRubberBand) {
        ChartView *view = qobject_cast<ChartView*>(parent());
        QRect rect = QRect(mRubberBandStart.toPoint(), view->mapFromScene(e->scenePos()));
        
        QPolygonF p = view->mapToScene(rect.normalized());
        QPainterPath path;
        path.addPolygon(p);
        setSelectionArea(path);
        
        mRubberBand->hide();
        delete mRubberBand;
        mRubberBand = 0;
    }
}

void CrochetScene::stitchModeMousePress(QGraphicsSceneMouseEvent* e)
{
    Q_UNUSED(e);
}

void CrochetScene::stitchModeMouseMove(QGraphicsSceneMouseEvent* e)
{
    if(e->buttons() != Qt::LeftButton)
        return;
    
    QGraphicsItem *gi = itemAt(e->scenePos());
    CrochetCell *c = qgraphicsitem_cast<CrochetCell*>(gi);
    if(!c)
        return;
    
    if(c->name() != mEditStitch)
        mUndoStack.push(new SetCellStitch(this, findGridPosition(c), mEditStitch));
}

void CrochetScene::stitchModeMouseRelease(QGraphicsSceneMouseEvent* e)
{
    Q_UNUSED(e);
    //FIXME: foreach(stitch in selection()) create an undo group event.
    if(mCurCell) {
        if(mCurCell->name() != mEditStitch)
            mUndoStack.push(new SetCellStitch(this, findGridPosition(mCurCell), mEditStitch));
    }
    
    mCurCell = 0;
}

void CrochetScene::angleModeMousePress(QGraphicsSceneMouseEvent *e)
{
    if(mCurCell) {
        qreal value = acos(mCurCell->transform().m11()) / M_PI * 180;
qDebug() << value;
        if(e->scenePos().x() < 0 && e->scenePos().y() >= 0)
            mCurCellRotation = 180 - value;
        else if(e->scenePos().x() < 0 && e->scenePos().y() < 0)
            mCurCellRotation = 180 - value;
        else
            mCurCellRotation = value;
        qDebug() << mCurCellRotation;
    }
   
}

void CrochetScene::angleModeMouseMove(QGraphicsSceneMouseEvent *e)
{
    if(!mCurCell)
        return;

    QPointF origin = mCurCell->mapToScene(32, 0);
    QPointF first = e->buttonDownScenePos(Qt::LeftButton);
    QPointF second = e->scenePos();
    QPointF rel1 = QPointF(first.x() - origin.x(), first.y() - origin.y());
    QPointF rel2 = QPointF(second.x() - origin.x(), second.y() - origin.y());
    qreal angle1 = scenePosToAngle(rel1);
    qreal angle2 = scenePosToAngle(rel2);

    qreal final = mCurCellRotation - (angle1 - angle2);
    
    mCurCell->setTransform(
        QTransform().translate(32,0).rotate(final).translate(-32, 0));
}

void CrochetScene::angleModeMouseRelease(QGraphicsSceneMouseEvent *e)
{
    Q_UNUSED(e);
    mCurCellRotation = 0;
}

void CrochetScene::strechModeMousePress(QGraphicsSceneMouseEvent* e)
{
    Q_UNUSED(e);
}

void CrochetScene::strechModeMouseMove(QGraphicsSceneMouseEvent* e)
{
    if(!mCurCell)
        return;
    
    QPointF start = e->buttonDownScenePos(Qt::LeftButton);
    QPointF cur = e->scenePos();
    qreal scale = (start.manhattanLength() - cur.manhattanLength()) / 64;

    QTransform trans = mCurCell->transform().scale(1, 1 + scale);
    mCurCell->setTransform(trans);
}

void CrochetScene::strechModeMouseRelease(QGraphicsSceneMouseEvent* e)
{
    Q_UNUSED(e);
}
