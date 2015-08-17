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
#include "colorlabel.h"

#include <QPainter>
#include <QDebug>

#include <QColorDialog>
#include <QPaintEvent>
#include <QMimeData>

#include <QCommonStyle>

ColorLabel::ColorLabel(QWidget *parent) :
    QLabel(parent),
    mColor(QColor(Qt::black))
{
    setAutoFillBackground(true);
    updateColor();
    setAcceptDrops(true);

}

void ColorLabel::setColor(QColor c)
{
    if(c == mColor)
        return;

    mColor = c;
    updateColor();
    emit colorChanged(c);
}

void ColorLabel::setText(const QString &text)
{
    QPainter p(this);
    int w = p.fontMetrics().width(text);
    setMinimumWidth(w + 4);
    QLabel::setText(text);
}

void ColorLabel::mouseReleaseEvent(QMouseEvent *ev)
{
    QLabel::mouseReleaseEvent(ev);
    selectColor();
}

void ColorLabel::dragEnterEvent(QDragEnterEvent *e)
{
    if(e->mimeData()->hasFormat("application/x-color")) {
        e->acceptProposedAction();
    }
}

void ColorLabel::dropEvent(QDropEvent *e)
{

    QColor c = e->mimeData()->colorData().value<QColor>();
    if(c.isValid()) {
        setColor(c);
    }
}

void ColorLabel::updateColor()
{

    QPalette pal = palette();
    pal.setBrush(QPalette::Window, QBrush(mColor));
    setPalette(pal);
}

void ColorLabel::selectColor()
{

    QColorDialog cDlg;
    QColor newColor = cDlg.getColor(mColor, this, text());

    if(newColor.isValid())
        setColor(newColor);
}

void ColorLabel::paintEvent(QPaintEvent *event)
{

    Q_UNUSED(event);

    QCommonStyle style;

    QPainter painter(this);
    QFont font = painter.font();
    font.setPointSize(9);
    painter.setPen(QPen(QColor(Qt::black)));
    painter.setFont(font);

    int txtWidth = painter.fontMetrics().width(text());
    int txtHeight = painter.fontMetrics().height();

    QRectF rect = QRectF((width() - txtWidth)/2 -2, (height() - txtHeight)/2 -2,
                         txtWidth+4, txtHeight+4);

    painter.setBrush(QBrush(QColor(255,255,255,128)));
    painter.setPen(QPen(QColor(255,255,255,128)));
    painter.drawRoundedRect(rect, 3, 3);

    style.drawItemText(&painter, rect.toRect(), Qt::AlignCenter, palette(),
                       true, text(), QPalette::Foreground);
}
