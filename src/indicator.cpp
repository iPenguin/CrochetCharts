/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "indicator.h"

#include <QDebug>

#include <QPainter>
#include "settings.h"
#include <QStyleOption>
#include <QKeyEvent>

Indicator::Indicator(QGraphicsItem* parent, QGraphicsScene* scene)
    : QGraphicsTextItem(parent, scene),
      highlight(true)
{
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsFocusable);
    setTextInteractionFlags(Qt::TextEditorInteraction);
    setZValue(150);
}

Indicator::~Indicator()
{
}

QRectF Indicator::boundingRect()
{
    QRectF rect = QGraphicsTextItem::boundingRect();
    rect.setLeft(rect.left() - 10);
    rect.setTop(rect.top() - 10);
    rect.setWidth(rect.width() + 10);
    rect.setHeight(rect.height() + 10);
    return rect;
}

void Indicator::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{

    QString style = Settings::inst()->value("chartRowIndicator").toString();
    QString color = Settings::inst()->value("chartIndicatorColor").toString();
    bool showOutline = Settings::inst()->value("showIndicatorOutline").toBool();

    if(highlight || showOutline) {
        painter->setPen(QColor(Qt::gray));
        painter->drawRect(option->rect);
    }

    if(style == "Dots" || style == "Dots and Text") {
        painter->setPen(QColor(color));
        painter->setBackgroundMode(Qt::OpaqueMode);
        painter->setBrush(QBrush(QColor(color)));
        painter->drawEllipse(-7,-7, 12,12);
        painter->setBackgroundMode(Qt::TransparentMode);
    }

    if(style == "Text" || style == "Dots and Text") {
        QStyleOptionGraphicsItem opt = *option;
        QGraphicsTextItem::paint(painter, &opt, widget);
    }

    if(option->state & QStyle::State_Selected) {
       painter->setPen(Qt::DashLine);
       painter->drawRect(option->rect);
       painter->setPen(Qt::SolidLine);
   }

}

void Indicator::focusInEvent(QFocusEvent *event)
{
    setTextInteractionFlags(Qt::TextEditorInteraction);
    QGraphicsTextItem::focusInEvent(event);
}

void Indicator::focusOutEvent(QFocusEvent* event)
{
    setTextInteractionFlags(Qt::NoTextInteraction);
    //emit lostFocus(this);
    QGraphicsTextItem::focusOutEvent(event);
}

void Indicator::keyReleaseEvent(QKeyEvent *event)
{
    event->accept();
    QGraphicsTextItem::keyReleaseEvent(event);
}
