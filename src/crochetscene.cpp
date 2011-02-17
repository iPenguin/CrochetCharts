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
#include "stitchcollection.h"
#include "stitchset.h"
#include "appinfo.h"
#include "crochetchartcommands.h"

CrochetScene::CrochetScene(QObject *parent)
    : QGraphicsScene(parent), mCurCell(0), mDiff(0.0), mMode(CrochetScene::StitchMode),
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

void CrochetScene::appendCell(int row, Cell *c)
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
    
    //TODO: abstract out setting the position to a seperate function: void setCellPos(Cell *c);
    int i = mGrid[row].count() -1;
    c->setPos(i*64, row*64);
    c->setToolTip(QString::number(i+1));
    //c->rotate(90);
    c->setColor(QColor(Qt::white));
    c->setObjectName("Cell Object: " + QString::number(i + 1));
    //emit rowChanged(row);
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

    QString st = stitch;
    Stitch* s = StitchCollection::inst()->findStitch(st);
    
    QList<Cell*> modelRow;
    for(int i = 0; i < columns; ++i) {
        c = new CrochetCell();
        connect(c, SIGNAL(stitchChanged(QString,QString)), this, SIGNAL(stitchChanged(QString,QString)));
        connect(c, SIGNAL(colorChanged(QString,QString)), this, SIGNAL(colorChanged(QString,QString)));
        c->setStitch(s);
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

QPointF CrochetScene::calcPoint(double radius, double angleInDegrees, QPointF origin)
{
    // Convert from degrees to radians via multiplication by PI/180
    double x = (double)(radius * cos(angleInDegrees * M_PI / 180)) + origin.x();
    double y = (double)(radius * sin(angleInDegrees * M_PI / 180)) + origin.y();
    return QPointF(x, y);
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

void CrochetScene::colorModeMouseMove(QGraphicsSceneMouseEvent* e)
{
    
}

void CrochetScene::colorModeMousePress(QGraphicsSceneMouseEvent* e)
{

    if(e->buttons() != Qt::LeftButton)
        return;
    
    QGraphicsItem *gi = itemAt(e->scenePos());
    CrochetCell *c = qgraphicsitem_cast<CrochetCell*>(gi);
    if(!c)
        return;
    
    mUndoStack.push(new SetCellColor(c, mEditBgColor));
    
}

void CrochetScene::colorModeMouseRelease(QGraphicsSceneMouseEvent* e)
{
    
}

void CrochetScene::gridModeMouseMove(QGraphicsSceneMouseEvent* e)
{
    
}

void CrochetScene::gridModeMousePress(QGraphicsSceneMouseEvent* e)
{
    
}

void CrochetScene::gridModeMouseRelease(QGraphicsSceneMouseEvent* e)
{
    
}

void CrochetScene::positionModeMouseMove(QGraphicsSceneMouseEvent* e)
{
    if(e->buttons() != Qt::LeftButton)
        return;
    
    if(!mCurCell)
        return;
    
    if(mDiff == 0) {
        QPointF origPos = e->buttonDownScenePos(Qt::LeftButton);
        QPointF curPos = e->scenePos();
        
        mDiff = origPos.manhattanLength() - curPos.manhattanLength();
    } else {
        //increase rotation speed over time...
        if(mDiff < 0)
            mDiff -= .5;
        else
            mDiff += .5;
    }

    mUndoStack.push(new SetCellRotation(mCurCell, mDiff));
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
    mDiff = 0;
}

void CrochetScene::positionModeMouseRelease(QGraphicsSceneMouseEvent* e)
{
    if(mCurCell)
        mCurCell = 0;
    mDiff = 0;
}

void CrochetScene::stitchModeMouseMove(QGraphicsSceneMouseEvent* e)
{
    
}

void CrochetScene::stitchModeMousePress(QGraphicsSceneMouseEvent* e)
{
    if(e->buttons() != Qt::LeftButton)
        return;
    
    QGraphicsItem *gi = itemAt(e->scenePos());
    CrochetCell *c = qgraphicsitem_cast<CrochetCell*>(gi);
    if(!c)
        return;
    
    mUndoStack.push(new SetCellStitch(c, mEditStitch));

}

void CrochetScene::stitchModeMouseRelease(QGraphicsSceneMouseEvent* e)
{
    
}
