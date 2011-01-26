/*************************************************\
| (c) 2011 Stitch Works Software                  |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "chartview.h"

#include <QWheelEvent>
#include <QDebug>

ChartView::ChartView(QWidget *parent)
    : QGraphicsView(parent)
{
}

ChartView::~ChartView()
{
}

void ChartView::mouseMoveEvent(QMouseEvent *event)
{
    QGraphicsView::mouseMoveEvent(event);
}

void ChartView::mousePressEvent(QMouseEvent *event)
{
    QGraphicsView::mousePressEvent(event);
}

void ChartView::mouseReleaseEvent(QMouseEvent *event)
{
    QGraphicsView::mouseReleaseEvent(event);
}

void ChartView::scrollContentsBy(int dx, int dy)
{
    QGraphicsView::scrollContentsBy(dx, dy);
}

void ChartView::wheelEvent(QWheelEvent *event)
{
    if(event->modifiers() && Qt::CTRL)
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