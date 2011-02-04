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

CrochetScene::CrochetScene(QObject *parent)
    : QGraphicsScene(parent), mCurCell(0), mDiff(0.0)
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
    c->rotate(90);
    c->setBgColor(QColor(Qt::white));
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

void CrochetScene::createChart(int rows, int cols)
{
    for(int i = 0; i < rows; ++i)
        createRow(i, cols);
    
    emit chartCreated(rows, cols);
}

void CrochetScene::createRow(int row, int columns)
{
    Cell *c = 0;

    QString st = Settings::inst()->value("defaultStitch").toString();
    Stitch* s = StitchCollection::inst()->masterStitchSet()->findStitch(st);
    
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
        c->setBgColor(QColor(Qt::white));
        c->rotate(90);
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

void CrochetScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(mouseEvent->buttons() != Qt::LeftButton)
        return;
    
    QGraphicsItem *gi = itemAt(mouseEvent->scenePos());
    CrochetCell *c = qgraphicsitem_cast<CrochetCell*>(gi);
    if(!c)
        return;

    mCurCell = c;
    mDiff = 0;
        
    QGraphicsScene::mouseMoveEvent(mouseEvent);
}

void CrochetScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    //FIXME: only send the event to the currently moving item.
    if(mouseEvent->buttons() != Qt::LeftButton)
        return;
    
    if(!mCurCell)
        return;

    if(mDiff == 0) {
        QPointF origPos = mouseEvent->buttonDownScenePos(Qt::LeftButton);
        QPointF curPos = mouseEvent->scenePos();
    
        mDiff = origPos.manhattanLength() - curPos.manhattanLength();
    } else {
        //increase speed slowly...
        if(mDiff < 0)
            mDiff -= .5;
        else
            mDiff += .5;
    }
    mCurCell->setTransform(QTransform().translate(32,32).rotate(mDiff).translate(-32, -32), true);
    //mCurCell->rotate(diff);
    
    QGraphicsScene::mouseMoveEvent(mouseEvent);
}

void CrochetScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(mCurCell)
        mCurCell = 0;
    mDiff = 0;
    
    QGraphicsScene::mouseMoveEvent(mouseEvent);
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
