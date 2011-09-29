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
      highlight(true)
{
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsFocusable);
    setZValue(150);
}

Indicator::~Indicator()
{
}

QRectF Indicator::boundingRect() const
{
    QRectF rect = QGraphicsTextItem::boundingRect();
    /*
    rect.setLeft(rect.left() - 7);
    rect.setTop(rect.top() - 7);
    rect.setWidth(rect.width() + 7);
    rect.setHeight(rect.height() + 7);
    */
    return rect;
}

QPainterPath Indicator::shape() const
{
    QPainterPath path = QGraphicsTextItem::shape();
    //path.addEllipse(-7, -7, 12, 12);
    return path;
}

void Indicator::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{

    QString style = Settings::inst()->value("chartRowIndicator").toString();
    QString color = Settings::inst()->value("chartIndicatorColor").toString();
    bool showOutline = Settings::inst()->value("showIndicatorOutline").toBool();

    QRect rect = option->rect;
    rect.setTop(7);
    rect.setLeft(7);
    
    if(option->state & (QStyle::State_Selected | QStyle::State_HasFocus)) {
        QPen pen = QPen(QColor(Qt::black));
        pen.setStyle(Qt::DashLine);
        painter->setPen(pen);
        painter->drawRect(rect);
    } else if(highlight || showOutline) {
        painter->setPen(QColor(Qt::gray));
        painter->drawRect(rect);
    }

    if(style == "Dots" || style == "Dots and Text") {
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setPen(QColor(color));
        painter->setBackgroundMode(Qt::OpaqueMode);
        painter->setBrush(QBrush(QColor(color)));
        painter->drawEllipse(0,0, 12,12);
        painter->setBackgroundMode(Qt::TransparentMode);
    }

    if(style == "Text" || style == "Dots and Text") {
        painter->setPen(QColor(Qt::black));
        //painter->drawText(rect, text());
        document()->drawContents(painter, rect);

        QTextBlock block = document()->findBlock(textCursor().position());
        QTextLine line = block.layout()->lineForTextPosition(textCursor().positionInBlock());
        qreal cursor_y_position = document()->documentLayout()->blockBoundingRect(block).y() + line.y();
        qreal line_height = line.height();
        qDebug("New cursor position: %.2f %.2f", cursor_y_position, line_height);

        QTextLayout l(text());
        l.beginLayout();

        l.endLayout();
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
    //eat delete and other keys so they don't delete this object by mistake.
    event->accept();
    QGraphicsTextItem::keyReleaseEvent(event);
}

void Indicator::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if(isSelected()) {
        if(textInteractionFlags() == Qt::NoTextInteraction) {
            setTextInteractionFlags(Qt::TextEditorInteraction);
        }
    }
    QGraphicsTextItem::mouseReleaseEvent(event);
}
