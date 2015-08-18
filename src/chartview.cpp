/****************************************************************************\
 Copyright (c) 2010-2014 Stitch Works Software
 Brian C. Milco <bcmilco@gmail.com>

 This file is part of Crochet Charts.

 Crochet Charts is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Crochet Charts is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Crochet Charts. If not, see <http://www.gnu.org/licenses/>.

 \****************************************************************************/
#include "chartview.h"

#include <QWheelEvent>
#include <QDebug>
#include <QScrollBar>

ChartView::ChartView(QWidget* parent)
    : QGraphicsView(parent),
	mSnapToGrid(false),
	mRows(1),
	mColumns(1),
	mWidth(64),
	mHeight(64),
	mPos(0,0),
	mSnapType(ChartView::Rows)
	
{
	setAcceptDrops(true);
}

ChartView::~ChartView()
{
}

void ChartView::mousePressEvent(QMouseEvent* event)
{    
    QGraphicsView::mousePressEvent(event);
}

void ChartView::mouseMoveEvent(QMouseEvent* event)
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
        
        bool isHorizLimit = false;
        isHorizLimit = (horizontalScrollBar()->value() == horizontalScrollBar()->minimum()) ? true : isHorizLimit;
        isHorizLimit = (horizontalScrollBar()->value() == horizontalScrollBar()->maximum()) ? true : isHorizLimit;

        bool isVertLimit = false;
        isVertLimit = (verticalScrollBar()->value() == verticalScrollBar()->minimum()) ? true : isVertLimit;
        isVertLimit = (verticalScrollBar()->value() == verticalScrollBar()->maximum()) ? true : isVertLimit;

        if((deltaX != 0 && !isHorizLimit) || (deltaY != 0 && !isVertLimit))
            emit scrollBarChanged(deltaX, deltaY);
    }
	
	if (mSnapToGrid) {
		QPoint newPos = mapFromScene(snapPosition(mapToScene(event->globalPos())));
		
		QMouseEvent snapEvent(event->type(), newPos, event->button(), event->buttons(), event->modifiers());
		QGraphicsView::mouseMoveEvent(&snapEvent);
	}
	else {
		QGraphicsView::mouseMoveEvent(event);
	}
}

void ChartView::mouseReleaseEvent(QMouseEvent* event)
{
    QGraphicsView::mouseReleaseEvent(event);
}

void ChartView::wheelEvent(QWheelEvent* event)
{
    if (event->modifiers() && Qt::CTRL)
        zoom(event->delta());
    else
        QGraphicsView::wheelEvent(event);
}

void ChartView::zoomIn()
{
    zoomLevel((transform().m11()*100) + 5);
    emit zoomLevelChanged(transform().m11()*100);
}

void ChartView::zoomOut()
{
    zoomLevel((transform().m11()*100) - 5);
    emit zoomLevelChanged(transform().m11()*100);
}

void ChartView::zoom(int mouseDelta)
{
    double scroll = mouseDelta / 120;
    int delta = 5 * scroll;
    zoomLevel((transform().m11()*100) + delta);
    emit zoomLevelChanged(transform().m11()*100);
}

void ChartView::zoomLevel(int percent)
{
    qreal pcent = percent / 100.0;
    if(pcent <= 0)
        pcent = 0.01;
    qreal diff = pcent / transform().m11();
    scale(diff, diff);
}

QPointF ChartView::snapPosition(QPointF pos) const
{
	return QPointF(0, 0);
}
