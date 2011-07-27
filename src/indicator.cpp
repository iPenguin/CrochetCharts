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

QRectF Indicator::boundingRect()
{
    QRectF rect = QGraphicsTextItem::boundingRect();
    rect.setTopLeft(QPointF(-5, -5));
    return rect;
}

void Indicator::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QString style = Settings::inst()->value("chartRowIndicator").toString();
    QString color = Settings::inst()->value("chartIndicatorColor").toString();
    if(style == "Dots" || style == "Dots and Text") {
        painter->setPen(QColor(color));
        painter->setBackgroundMode(Qt::OpaqueMode);
        painter->setBrush(QBrush(QColor(color)));
        painter->drawEllipse(-5,-5, 5,5);
    }
    
    if(style == "Text" || style == "Dots and Text")
        QGraphicsTextItem::paint(painter, option, widget);
}

void Indicator::focusOutEvent(QFocusEvent *event)
{
    setTextInteractionFlags(Qt::NoTextInteraction);
    //emit lostFocus(this);
    QGraphicsTextItem::focusOutEvent(event);
}
