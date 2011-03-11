/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "crochetcell.h"

#include <QPainter>
#include <QDebug>

#include <QApplication>
#include <qpainter.h>
#include <qstyleoption.h>

CrochetCell::CrochetCell()
    : mHighlight(false)
{
}

CrochetCell::~CrochetCell()
{
}

QGraphicsItem::GraphicsItemFlags CrochetCell::flags() const
{
    return Cell::flags() | QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable;
}

QRectF CrochetCell::boundingRect () const
{
    return Cell::boundingRect();
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
