/*************************************************\
| (c) 2010-2011 Stitch Works Software             |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "crochetscene.h"

#include "crochetcell.h"

#include <QFontMetrics>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsSceneEvent>

#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include <math.h>

#include <QDebug>

#include "settings.h"
#include "crochetdatamodel.h"

CrochetScene::CrochetScene(QObject *parent)
    : QGraphicsScene(parent)
{
    mModel = new CrochetDataModel(this);
    
    mStitchWidth = 64;

    this->createRow(0, 8);
    this->createRow(1, 14);
    this->createRow(2, 20);

    this->initDemoBackground();
}

void CrochetScene::initDemoBackground()
{

    if(Settings::inst()->isDemoVersion()) {

        double fontSize = 32.0;
        QFont demoFont = QFont();
        demoFont.setPointSize(fontSize);
        QString demoString = "Stitch Works Software - Demo Version  -  ";

        QFontMetrics fm = QFontMetrics(demoFont);
        double stringWidth = fm.width(demoString);

        QGraphicsSimpleTextItem *demoText;
        QRectF rect = this->sceneRect();
        double demoRows = rect.height() / fontSize;
        demoRows = demoRows /2.0;

        double demoCols = rect.width() / stringWidth;

        for(int c = 0; c < ceil(demoCols); ++c) {
            for(int i = 0; i < ceil(demoRows); ++i) {
                demoText = this->addSimpleText(demoString, demoFont);
                demoText->setBrush(QBrush(QColor(Qt::lightGray)));
                QPointF point = QPointF(rect.left() + c*stringWidth , rect.top() + i*(2*fontSize));
                demoText->setPos(point);
                demoText->setZValue(-1);
            }
        }

        //restore original rect. letting the demo text overflow off the scene.
        this->setSceneRect(rect);
    }
}

void CrochetScene::createRow(int row, int columns)
{
    //TODO: use c->setSharedRenderer(QSvgRenderer *renderer); and the Stitch *s->svgRenderer(); to draw the stitches?
    Cell *c;

    for(int i = 0; i < columns; ++i) {
        c = new CrochetCell(":/stitches/chain.svg");
        addItem(c);
        mModel->setCell(row, i, c);
        c->setPos(i*64, row*64);
        c->setToolTip(QString::number(i+1));
        c->rotate(90);
        c->setObjectName("Cell Object: " + QString::number(i + 1));
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
    
    QGraphicsScene::mouseMoveEvent(mouseEvent);
}

void CrochetScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    //FIXME: only send the event to the currently moving item.
    if(mouseEvent->buttons() != Qt::LeftButton)
        return;
    
    if(!mCurCell)
        return;
    
    QPointF origPos = mouseEvent->buttonDownScenePos(Qt::LeftButton);
    QPointF curPos = mouseEvent->scenePos();
    
    qreal diff = origPos.manhattanLength() - curPos.manhattanLength();
    mCurCell->rotate(diff);
    
    QGraphicsScene::mouseMoveEvent(mouseEvent);
}

void CrochetScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    mCurCell = 0;
    QGraphicsScene::mouseMoveEvent(mouseEvent);
}

void CrochetScene::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{

    QGraphicsScene::dragEnterEvent(event);
}

void CrochetScene::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
{

    QGraphicsScene::dragLeaveEvent(event);
}

void CrochetScene::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{

    QGraphicsScene::dragMoveEvent(event);
}

void CrochetScene::dropEvent(QGraphicsSceneDragDropEvent *event)
{

    QGraphicsScene::dropEvent(event);
}

void CrochetScene::save(QXmlStreamWriter *stream)
{
    qDebug() << "CrochetScene::save()";

    QList<QGraphicsItem*> items = this->items();
    foreach(QGraphicsItem* itm, items) {
        CrochetCell* c = qgraphicsitem_cast<CrochetCell*>(itm);
        if(!c)
            return;
        c->save(stream);
    }
/*
    int rows = mModel->rowCount();

    for(int row = 0; row < rows; ++row) {
        int cols = mModel->columnCount(row);
        for(int col = 0; col < cols; ++col) {
            CrochetCell *c = qobject_cast<CrochetCell*>(mModel->cell(row, col));
            c->save(stream);
        }
    }
*/
}

void CrochetScene::load(QXmlStreamReader* stream)
{
    if(stream->isStartElement())
    {
        QString tag = stream->name().toString();

        if(tag == "stitch")

    }
    stream->readNext();

}
