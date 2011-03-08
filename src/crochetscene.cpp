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
    : QGraphicsScene(parent), mCurCell(0), mDiff(QSizeF(0,0)), mMode(CrochetScene::StitchMode),
    mEditStitch("ch"), mEditFgColor(QColor(Qt::black)), mEditBgColor(QColor(Qt::white))
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
    
    //TODO: abstract out setting the position to a seperate function: void setCellPos(Cell *c);
    int i = mGrid[row].count() -1;
    c->setPos(i*64, row*64);
    c->setToolTip(QString::number(i+1));
    c->setColor(QColor(Qt::white));
    c->setObjectName("Cell Object: " + QString::number(i + 1));

    if(fromSave)
        emit rowChanged(row);
}

void CrochetScene::insertCell(int row, int colBefore, Cell *c)
{
    Q_ASSERT(mGrid.count() > row);
    
    addItem(c);
    mGrid[row].insert(colBefore, c);
    emit rowChanged(row);
}

void CrochetScene::createChart(int rows, int cols, QString stitch)
{
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
        c->setPos(i*64, row*64);
        c->setToolTip(QString::number(i+1));
        c->setColor(QColor(Qt::white));
        //c->rotate(90);
        c->setObjectName("Cell Object: " + QString::number(i + 1));
    }
    mGrid.append(modelRow);
    //emit rowChanged(row);
    emit rowAdded(row);
}

/*
    int rowC = 8;
    //FIXME: less then 8 stitches gives funny rows.
    double widthInDegrees = 360.0 / columns;
    double circumference = ((rowC + (row*6)) - (rowC * 1/3)) * mStitchWidth;  //(columns - (rowC * 1/3)) * mStitchWidth;
    double diameter = circumference / M_PI;
    double radius = diameter / 2;
    qDebug() << widthInDegrees << circumference << diameter << radius;
  
    Cell *c;
 
    for(int i = 0; i < columns; ++i) {
        double degrees = (widthInDegrees*i) - (widthInDegrees/2);
        QPointF finish = calcPoint(radius, degrees, QPointF(0,0));
        qDebug() << degrees << finish;
        c = new CrochetCell(":/stitches/chain.svg");
        addItem(c);
        c->setPos(finish);
        c->setToolTip(QString::number(i+1));
        c->rotate(degrees + (widthInDegrees/2));
    }
*/
QPointF CrochetScene::calcPoint(double radius, double angleInDegrees, QPointF origin)
{
    // Convert from degrees to radians via multiplication by PI/180
    double x = (double)(radius * cos(angleInDegrees * M_PI / 180)) + origin.x();
    double y = (double)(radius * sin(angleInDegrees * M_PI / 180)) + origin.y();
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
    
    mCurCell->setTransform(QTransform().translate(deltaX, deltaY));

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
