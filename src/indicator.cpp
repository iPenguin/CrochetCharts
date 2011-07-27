/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "indicator.h"

#include <QPainter>
#include "settings.h"
#include <QStyleOption>

Indicator::Indicator(QGraphicsItem *parent, QGraphicsScene *scene)
    : QGraphicsTextItem(parent, scene)
{
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
}

Indicator::~Indicator()
{
}

void Indicator::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{

    QString color = Settings::inst()->value("chartIndicatorColor").toString();
    painter->setPen(QColor(color));
    painter->setBackgroundMode(Qt::OpaqueMode);
    painter->setBrush(QBrush(QColor(color)));
    painter->drawEllipse(1,1, 10,10);

    QGraphicsTextItem::paint(painter, option, widget);
}

void Indicator::focusOutEvent(QFocusEvent *event)
{
    setTextInteractionFlags(Qt::NoTextInteraction);
    //emit lostFocus(this);
    QGraphicsTextItem::focusOutEvent(event);
}
