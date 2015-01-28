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
#include "stitchpalettedelegate.h"

#include "stitch.h"
#include <QPainter>
#include <QPixmap>

#include <QDebug>
#include <QSvgRenderer>

#include <QSortFilterProxyModel>

StitchPaletteDelegate::StitchPaletteDelegate(QWidget* parent)
    : QStyledItemDelegate(parent)
{
}

void StitchPaletteDelegate::paint(QPainter* painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    int pad = 5;

    const QSortFilterProxyModel* model =  static_cast<const QSortFilterProxyModel*>(index.model());
    QModelIndex idx = model->mapToSource(index);
    
    Stitch* s = static_cast<Stitch*>(idx.internalPointer());
    QRect rect = option.rect;

    if(option.state & QStyle::State_Selected)
        painter->fillRect(option.rect, option.palette.highlight());
    else if(option.state & QStyle::State_MouseOver)
        painter->fillRect(option.rect, option.palette.highlight().color().light(190));

    if(index.column() == 0) {
        painter->drawText(rect.left() + pad, rect.top() + option.fontMetrics.height(), s->name());  
    } else if(index.column() == 1) {
        qreal size;
        if(s->width() > 32 || s->height() > 32)
            size = 32;
        else
            size = s->width();

        QPixmap pix = (s->renderPixmap()->scaled(size, size, Qt::KeepAspectRatio,Qt::SmoothTransformation));
        painter->drawPixmap(rect.left() + pad, rect.top() + pad, pix);      
    }
}

QSize StitchPaletteDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{   
    Q_UNUSED(option);
    
    if(!index.isValid())
        return QSize();

    const QSortFilterProxyModel* model =  static_cast<const QSortFilterProxyModel*>(index.model());
    QModelIndex idx = model->mapToSource(index);
    Stitch* s = static_cast<Stitch*>(idx.internalPointer());
                
    if(!s)  
        return QSize();

    QString text = "";

    switch(index.column()) {
        case 0:
            text = s->name();
            break;
        case 1:
            text = s->file(); //TODO: make this the icon. 32x32.
            break;
        case 2:
            text = s->description();
            break;
        case 3:
            text = s->category();
            break;
        case 4:
            text = s->wrongSide();
            break;
        default:
            text = "";
            break;
    }

    if(index.column() > 1) {
        return QSize(0,0);
    }

    return QSize(100, 48);
}
