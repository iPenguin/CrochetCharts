/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "indicator.h"

#include <QPainter>
#include "settings.h"

Indicator::Indicator()
{
    setFlag(QGraphicsItem::ItemIsSelectable);
}

Indicator::~Indicator()
{
}

QRectF Indicator::boundingRect() const
{
    return QRectF(0,0,20,20);
}

void Indicator::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    QString color = Settings::inst()->value("chartIndicatorColor").toString();
    painter->setPen(QColor(color));
    painter->setBackgroundMode(Qt::OpaqueMode);
    painter->setBrush(QBrush(QColor(color)));
    painter->drawEllipse(1,1, 20,20);

    //QGraphicsTextItem::paint(painter, option, widget);
    
}
