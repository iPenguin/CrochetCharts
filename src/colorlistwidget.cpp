/****************************************************************************\
 Copyright (c) 2010-2014 Stitch Works Software
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
#include "colorlistwidget.h"

#include <QDrag>
#include <QMimeData>
#include <QMouseEvent>
#include <QApplication>
#include <QPainter>

#include <QDebug>

ColorListWidget::ColorListWidget(QWidget *parent) :
    QListWidget(parent),
    mDragStart(QPointF(0,0))
{
    setSelectionBehavior(QAbstractItemView::SelectItems);
    setSelectionMode(QAbstractItemView::SingleSelection);

}

void ColorListWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
        mDragStart = event->pos();

    QListWidget::mousePressEvent(event);
/*
    if(event->button() == Qt::RightButton) {

        emit bgColorSelected(currentIndex());
        event->accept();
    }
*/
}

void ColorListWidget::mouseMoveEvent(QMouseEvent *e)
{
    if (!(e->buttons() & Qt::LeftButton))
        return;

    if ((e->pos() - mDragStart).manhattanLength()
        < QApplication::startDragDistance())
        return;

    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData;

    QColor c = QColor(currentIndex().data(Qt::ToolTipRole).toString());
    QVariant v = c;

    mimeData->setText(c.name());
    mimeData->setColorData(v);
    drag->setMimeData(mimeData);

    /*Qt::DropAction dropAction =*/ drag->exec(Qt::CopyAction | Qt::MoveAction);

    QListWidget::mouseMoveEvent(e);
}

QPixmap ColorListWidget::drawColorBox(QColor color, QSize size)
{
    QPixmap pix = QPixmap(size);
    QPainter p;
    p.begin(&pix);

    if(color.isValid()) {
        p.fillRect(QRect(QPoint(0, 0), size), QColor(color));
    } else {
        p.fillRect(QRect(QPoint(0,0), size), QColor(Qt::white));
        QPen pen;
        pen.setColor(QColor(Qt::black));
        pen.setWidth(3);
        p.setPen(pen);
        p.drawLine(0,0, size.width(),size.height());
        p.drawLine(0,size.height(), size.width(),0);
    }

    p.drawRect(0, 0, size.width() - 1, size.height() - 1);
    p.end();

    return pix;
}

void ColorListWidget::dragEnterEvent(QDragEnterEvent *e)
{
    //Don't accept generic abstract model items.
    if(e->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist")) {
        e->dropAction();
        return;
    }

    QListWidget::dragEnterEvent(e);
}
