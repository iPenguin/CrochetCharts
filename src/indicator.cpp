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
#include <QGraphicsSceneEvent>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextBlock>
#include <QAbstractTextDocumentLayout>
#include <QTextLayout>

Indicator::Indicator(QGraphicsItem* parent, QGraphicsScene* scene)
    : QGraphicsTextItem(parent, scene),
      highlight(false)
{
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsFocusable);
    setZValue(150);

    mStyle = Settings::inst()->value("chartRowIndicator").toString();
}

Indicator::~Indicator()
{
}

QRectF Indicator::boundingRect() const
{
    QRectF rect = QGraphicsTextItem::boundingRect();
    return rect;
}

QPainterPath Indicator::shape() const
{
    QPainterPath path = QGraphicsTextItem::shape();
    return path;
}

void Indicator::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{

    QString color = Settings::inst()->value("chartIndicatorColor").toString();

    if(option->state & QStyle::State_HasFocus) {
        QGraphicsTextItem::paint(painter, option, widget);
    } else {

        document()->setDocumentMargin(10);

        if(mStyle == "Dots" || mStyle == "Dots and Text") {
            painter->setRenderHint(QPainter::Antialiasing);
            painter->setPen(QColor(color));
            painter->setBackgroundMode(Qt::OpaqueMode);
            painter->setBrush(QBrush(QColor(color)));
            painter->drawEllipse(0,0, 10,10);
            painter->setBackgroundMode(Qt::TransparentMode);
        }

        if(mStyle == "Dots" && (option->state & (QStyle::State_Selected | QStyle::State_HasFocus))) {
            QPen pen = QPen(QColor(Qt::black));
            pen.setStyle(Qt::DashLine);
            painter->setPen(pen);
            QRect r = option->rect;
            r.setBottomRight(QPoint(10,10));
            painter->drawRect(r);
        }

        if(mStyle == "Text" || mStyle == "Dots and Text") {
            QGraphicsTextItem::paint(painter, option, widget);
        }
    }
}

void Indicator::focusInEvent(QFocusEvent* event)
{
    QGraphicsTextItem::focusInEvent(event);
    emit gotFocus(this);
}

void Indicator::focusOutEvent(QFocusEvent* event)
{
    QGraphicsTextItem::focusOutEvent(event);
    setTextInteractionFlags(Qt::NoTextInteraction);
    emit lostFocus(this);
}

void Indicator::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Escape)
        setTextInteractionFlags(Qt::NoTextInteraction);

    //eat delete and other keys so they don't delete this object by mistake.
    event->accept();
    QGraphicsTextItem::keyReleaseEvent(event);
}

void Indicator::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{

    QGraphicsTextItem::mouseReleaseEvent(event);
}
