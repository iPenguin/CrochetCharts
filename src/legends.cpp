/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "legends.h"
#include <QPainter>

#include <QDebug>

ColorLegend::ColorLegend(QGraphicsItem *parents)
    : QGraphicsWidget(parent)
{
    qDebug() << "create ColorLegend";
    update();
}

ColorLegend::~ColorLegend()
{
}


void ColorLegend::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0)
{

    painter->drawText(0,0, "test");
}
