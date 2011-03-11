/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "chartview.h"

#include <QWheelEvent>
#include <QDebug>
#include <QScrollBar>

ChartView::ChartView(QWidget *parent)
    : QGraphicsView(parent), mRubberBand(0), mRubberBandStart(QPoint(0,0))
{
    //DONT USE THESE IF the mousePress & move & release code is in use.
    //setDragMode(QGraphicsView::RubberBandDrag);
    //setRubberBandSelectionMode(Qt::IntersectsItemBoundingRect);
}

ChartView::~ChartView()
{
}

void ChartView::mousePressEvent(QMouseEvent *event)
{
    //FIXME: somehow this code has to be set only when the modes are correct.
    //move code into the scene and static_cast() view->... ??
    if(event->buttons() == Qt::LeftButton) {
        mRubberBandStart = event->pos();
        if(!mRubberBand)
            mRubberBand = new QRubberBand(QRubberBand::Rectangle, this);
        mRubberBand->setGeometry(QRect(mRubberBandStart, QSize()));
        mRubberBand->show();
    }
    
    QGraphicsView::mousePressEvent(event);
}

void ChartView::mouseMoveEvent(QMouseEvent *event)
{
    int delta = 5;

    if(event->buttons() == Qt::LeftButton) {
        
        if(event->pos().x() < delta) {
            int diff = horizontalScrollBar()->value() - horizontalScrollBar()->minimum();
            if(diff < delta)
                delta = diff;
            
            horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta);
            mRubberBandStart.setX(mRubberBandStart.x() + delta);
            
        } else if (event->pos().x() > viewport()->width() - delta) {
            int diff = horizontalScrollBar()->maximum() - horizontalScrollBar()->value();
            if(diff < delta)
                delta = diff;
            
            horizontalScrollBar()->setValue(horizontalScrollBar()->value() + delta);
            mRubberBandStart.setX(mRubberBandStart.x() - delta);
        }

        delta = 5;
        if(event->pos().y() < delta) {
            int diff = verticalScrollBar()->value() - verticalScrollBar()->minimum();
            if(diff < delta)
                delta = diff;
            
            verticalScrollBar()->setValue(verticalScrollBar()->value() - delta);
            mRubberBandStart.setY(mRubberBandStart.y() + delta);
            
        } else if( event->pos().y() > viewport()->height() - delta) {
            int diff = verticalScrollBar()->maximum() - verticalScrollBar()->value();
            if(diff < delta)
                delta = diff;
            
            verticalScrollBar()->setValue(verticalScrollBar()->value() + delta);
            mRubberBandStart.setY(mRubberBandStart.y() - delta);
        }

        if(mRubberBand)
            mRubberBand->setGeometry(QRect(mRubberBandStart, event->pos()).normalized());
    }

    QGraphicsView::mouseMoveEvent(event);
}

void ChartView::mouseReleaseEvent(QMouseEvent *event)
{
    if(mRubberBand) {
        mRubberBand->hide();
        emit selectionChanged(mapToScene(QRect(mRubberBandStart, event->pos()).normalized()));
    }
    
    QGraphicsView::mouseReleaseEvent(event);
}

void ChartView::wheelEvent(QWheelEvent *event)
{
    if (event->modifiers() && Qt::CTRL)
        zoom(event->delta());
    else
        QGraphicsView::wheelEvent(event);
}

void ChartView::zoom(int mouseDelta)
{
    double scroll = mouseDelta / 120;
    scroll /= 10;
    scroll += 1;
    scale(scroll, scroll);
}
