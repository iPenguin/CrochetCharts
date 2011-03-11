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

#include "settings.h"
#include "stitchset.h"
#include "appinfo.h"
#include "crochetchartcommands.h"

CrochetScene::CrochetScene(QObject *parent)
    : QGraphicsScene(parent), mCurCell(0), mDiff(QSizeF(0,0)), mHighlightCell(0),
    mRowSpacing(8), mStyle(CrochetScene::Flat),
    mMode(CrochetScene::StitchMode), mEditStitch("ch"),
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
        QString demoString = AppInfo::demoString;

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

Cell* CrochetScene::cell(int row, int column)
{
    Q_ASSERT(mGrid.count() > row);
    if(mGrid[row].count() <= column)
        return 0;

    return mGrid[row][column];
}

Cell* CrochetScene::cell(QPoint position)
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

void CrochetScene::appendCell(int row, Cell *c, bool fromSave)
{
    //append any missing rows.
    if(mGrid.count() <= row) {
        for(int i = mGrid.count(); i < row + 1; ++i) {
            QList<Cell*> row;
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

void CrochetScene::insertCell(int row, int colBefore, Cell *c)
{
    Q_ASSERT(mGrid.count() > row);
    
    addItem(c);
    mGrid[row].insert(colBefore, c);
    emit rowChanged(row);
}

void CrochetScene::setCellPosition(int row, int column, Cell *c, int columns)
{
    if(mStyle == CrochetScene::Round) {
        double widthInDegrees = 360.0 / columns;
        double circumference = ((row+1)*mRowSpacing) * mStitchWidth;
        double diameter = circumference / M_PI;
        double radius = diameter / 2;
        
        double degrees = widthInDegrees*column;
        QPointF finish = calcPoint(radius, degrees, QPointF(0,0));        
        c->setPos(finish.x() - 32, finish.y() - 16);
        c->setTransform(QTransform().translate(32,16).rotate(degrees + 90).translate(-32, -16));
        c->setToolTip(QString::number(column+1));
        
    } else {
        c->setPos(column*64, row*64);
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
        Cell *c = mGrid[row].at(i);
        setCellPosition(row, i, c, columns);
    }
}

void CrochetScene::createChart(CrochetScene::ChartStyle style, int rows, int cols, QString stitch)
{
    mStyle = style;
    for(int i = 0; i < rows; ++i)
            createRow(i, cols, stitch);
    
    emit chartCreated(rows, cols);
}

void CrochetScene::createRow(int row, int columns, QString stitch)
{
    Cell *c = 0;
    
    QList<Cell*> modelRow;
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

//FIXME: currently unused combine with the createRow function
void CrochetScene::createRoundRow(int row, int columns, QString stitch)
{
    Cell *c;
    QList<Cell*> modelRow;
    for(int i = 0; i < columns; ++i) {
        c = new CrochetCell();
        c->setStitch(stitch, (row %2));

        addItem(c);
        modelRow.append(c);
        c->setColor(QColor(Qt::white));
        setCellPosition(row, i, c);
    }
    mGrid.append(modelRow);
    emit rowAdded(row);
}

int CrochetScene::getClosestRow(QPointF mousePosition)
{
    qreal circumference = sqrt(mousePosition.x()*mousePosition.x() + mousePosition.y()*mousePosition.y()) * 2 * M_PI;
    qreal temp = circumference / mStitchWidth;
    int row = round(temp / mRowSpacing) - 1;
    
    return row;
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
    qDebug() << "selection changed, do something useful!" << selection <<  selectedItems().count();

    QPainterPath path;
    path.addPolygon(selection);
    setSelectionArea(path);

    
    qDebug() << selectedItems().count();
}

void CrochetScene::mousePressEvent(QGraphicsSceneMouseEvent *e)
{
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
            positionModeMousePress(e);
            break;
        default:
            break;
    }
            
    QGraphicsScene::mousePressEvent(e);
}

void CrochetScene::mouseMoveEvent(QGraphicsSceneMouseEvent *e)
{
    highlightCell(e);
    
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
            positionModeMouseMove(e);
            break;
        default:
            break;
    }
    
    QGraphicsScene::mouseMoveEvent(e);
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
        default:
            break;
    }

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

    if(e->buttons() != Qt::LeftButton)
        return;
    
    QGraphicsItem *gi = itemAt(e->scenePos());
    CrochetCell *c = qgraphicsitem_cast<CrochetCell*>(gi);
    if(!c)
        return;

    mCurCell = c;
    
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
    if(mCurCell) {
        if(mCurCell->color() != mEditBgColor)
            mUndoStack.push(new SetCellColor(this, findGridPosition(mCurCell), mEditBgColor));
    }
    
    mCurCell = 0;
}

void CrochetScene::gridModeMousePress(QGraphicsSceneMouseEvent* e)
{
    
}

void CrochetScene::gridModeMouseMove(QGraphicsSceneMouseEvent* e)
{
    
}

void CrochetScene::gridModeMouseRelease(QGraphicsSceneMouseEvent* e)
{
    
}

void CrochetScene::positionModeMousePress(QGraphicsSceneMouseEvent* e)
{
    if(e->buttons() == Qt::RightButton && mStyle == CrochetScene::Round) {
        int row = getClosestRow(e->scenePos());
        redistributeCells(row);
    }
        
    if(e->buttons() != Qt::LeftButton)
        return;
    
    QGraphicsItem *gi = itemAt(e->scenePos());
    CrochetCell *c = qgraphicsitem_cast<CrochetCell*>(gi);
    if(!c)
        return;
    
    mCurCell = c;
    mDiff = QSizeF(mCurCell->transform().dx(), mCurCell->transform().dy());
}

void CrochetScene::positionModeMouseMove(QGraphicsSceneMouseEvent* e)
{
    if(e->buttons() != Qt::LeftButton)
        return;
    
    if(!mCurCell)
        return;

    QPointF curPos = e->scenePos();

    qreal dx = 0, dy = 0;
    if(!mDiff.isNull()) {
        dx = mDiff.width();
        dy = mDiff.height();
    }

    qreal deltaX = dx - (e->buttonDownScenePos(Qt::LeftButton).x() - curPos.x());
    qreal deltaY = dy - (e->buttonDownScenePos(Qt::LeftButton).y() - curPos.y());

    if(deltaX > 64) deltaX = 64;
    if(deltaX < -64) deltaX = -64;

    if(deltaY > 64) deltaY = 64;
    if(deltaY < -64) deltaY = -64;

    //snap to grid.
    if(abs(deltaX) < 4) deltaX = 0;
    if(abs(deltaY) < 4) deltaY = 0;
    
    QTransform trans = mCurCell->transform();
    QTransform newTrans;
    newTrans.setMatrix(trans.m11(), trans.m12(), trans.m13(),
                       trans.m21(), trans.m22(), trans.m23(),
                       deltaX,      deltaY,      trans.m33());
    mCurCell->setTransform(newTrans);

}

void CrochetScene::positionModeMouseRelease(QGraphicsSceneMouseEvent* e)
{
    if(mCurCell)
        mCurCell = 0;
    mDiff.setHeight(0);
    mDiff.setWidth(0);
}

void CrochetScene::stitchModeMousePress(QGraphicsSceneMouseEvent* e)
{
    if(e->buttons() != Qt::LeftButton)
        return;
    
    QGraphicsItem *gi = itemAt(e->scenePos());
    CrochetCell *c = qgraphicsitem_cast<CrochetCell*>(gi);
    if(!c)
        return;

    mCurCell = c;
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
    if(mCurCell) {
        if(mCurCell->name() != mEditStitch)
            mUndoStack.push(new SetCellStitch(this, findGridPosition(mCurCell), mEditStitch));
    }
    
    mCurCell = 0;
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
