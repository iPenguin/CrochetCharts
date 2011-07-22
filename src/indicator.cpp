/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "indicator.h"

#include <QPainter>

Indicator::Indicator()
{
}

Indicator::~Indicator()
{
}

QRectF Indicator::boundingRect() const
{
    return QRectF(0,0,21,21);
}

void Indicator::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->drawEllipse(1,1, 20,20);

}
