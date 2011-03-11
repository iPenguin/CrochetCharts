/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "crochetcell.h"

#include <QPainter>
#include <QDebug>

#include <QApplication>
#include <QPainter>
#include <QStyleOption>

CrochetCell::CrochetCell()
    : mHighlight(false)
{
    setFlag(QGraphicsItem::ItemIsMovable);
}

void CrochetCell::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if(color() != Qt::white)
        painter->fillRect(option->rect, color());
    if(mHighlight)
        painter->fillRect(option->rect, option->palette.highlight());
    
    if(stitch()->isSvg())
        Cell::paint(painter, option, widget);
    else
        painter->drawPixmap(option->rect.x(), option->rect.y(), *(stitch()->renderPixmap()));
    
    QRect rect =QRect(option->rect.x() + (option->rect.width()/2), option->rect.y() + (option->rect.height()/2),
                      (option->rect.width()/2), (option->rect.height()/2));
}
