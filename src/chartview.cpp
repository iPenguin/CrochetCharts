/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "chartview.h"

#include <QWheelEvent>
#include <QDebug>
#include <QScrollBar>

ChartView::ChartView(QWidget *parent)
    : QGraphicsView(parent)
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
    QGraphicsView::mousePressEvent(event);
}

void ChartView::mouseMoveEvent(QMouseEvent *event)
{
    int deltaX = 0;
    int deltaY = 0;

    if(event->buttons() == Qt::LeftButton) {
        
        if(event->pos().x() < 5) {
            int diff = horizontalScrollBar()->value() - horizontalScrollBar()->minimum();
            if(diff < deltaX)
                deltaX = -diff;
            else
                deltaX = -5;
            
        } else if (event->pos().x() > viewport()->width() - 5) {
            int diff = horizontalScrollBar()->maximum() - horizontalScrollBar()->value();
            if(diff < deltaX)
                deltaX = diff;
            else
                deltaX = 5;
        }

        horizontalScrollBar()->setValue(horizontalScrollBar()->value() + deltaX);
        
        if(event->pos().y() < 5) {
            int diff = verticalScrollBar()->value() - verticalScrollBar()->minimum();
            if(diff < deltaY)
                deltaY = -diff;
            else
                deltaY = -5;
            
        } else if( event->pos().y() > viewport()->height() - 5) {
            int diff = verticalScrollBar()->maximum() - verticalScrollBar()->value();
            if(diff < deltaY)
                deltaY = diff;
            else
                deltaY = 5;
        }
        
        verticalScrollBar()->setValue(verticalScrollBar()->value() + deltaY);
        
        if(deltaX > 0 && deltaY > 0)
            emit scrollBarChanged(deltaX, deltaY);
    }
    
    QGraphicsView::mouseMoveEvent(event);
}

void ChartView::mouseReleaseEvent(QMouseEvent *event)
{
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
