/*************************************************\
| (c) 2010 Stitch Works Software                  |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "crochetcell.h"

#include <QPainter>
#include <QDebug>

#include <QApplication>
#include <qpainter.h>
#include <qstyleoption.h>

CrochetCell::CrochetCell()
{
}

CrochetCell::~CrochetCell()
{
}

QRectF CrochetCell::boundingRect () const
{
    return Cell::boundingRect();
}

void CrochetCell::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Cell::paint(painter, option, widget);
    painter->drawText(option->rect.x(), option->rect.y(), toolTip());
}

int CrochetCell::type () const
{
    return CrochetCell::Type;
}
