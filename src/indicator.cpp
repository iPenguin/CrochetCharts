/****************************************************************************\
 Copyright (c) 2011-2014 Stitch Works Software
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
#include "scene.h"
#include "crochetchartcommands.h"
#include "ChartItemTools.h"

Indicator::Indicator(QGraphicsItem* parent, QGraphicsScene* scene)
    : QGraphicsTextItem(parent),
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

QString Indicator::text() {
	return toPlainText();
}

void Indicator::setText(QString t)
{
	setPlainText(t);
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
			painter->setFont(font());
            QGraphicsTextItem::paint(painter, option, widget);
        }
    }
}

void Indicator::focusInEvent(QFocusEvent* event)
{
	oldText = this->text();
    QGraphicsTextItem::focusInEvent(event);
    emit gotFocus(this);
}

void Indicator::focusOutEvent(QFocusEvent* event)
{
    QGraphicsTextItem::focusOutEvent(event);
    setTextInteractionFlags(Qt::NoTextInteraction);
	
	//if the old text is different from the current text, add it to the undo stack
	if (oldText.compare(text()) != 0) {
		Scene* s =  dynamic_cast<Scene*>(scene());
		if (s) {
			s->undoStack()->push(new SetIndicatorText(this, oldText, text()));
		}
	}
	
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
