/*************************************************\
| (c) 2010-2011 Stitch Works Software             |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "cell.h"

#include <QDebug>

Cell::Cell(QGraphicsItem *parent) :
    QGraphicsSvgItem(parent)
{
//    this->setCacheMode(QGraphicsItem::ItemCoordinateCache, QSize(32,64));

}

Cell::~Cell()
{
}

QRectF Cell::boundingRect() const
{
    return QGraphicsSvgItem::boundingRect();
}

void Cell::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QGraphicsSvgItem::paint(painter, option, widget);
}

void Cell::setStitch(Stitch* s)
{
    if(mStitch != s) {
        mStitch = s;
        setSharedRenderer(s->renderSvg());
    }
}

int Cell::type() const
{
    return Cell::Type;
}

void Cell::setRotation(qreal rotation)
{
    if(mRotation != rotation) {
        mRotation = rotation;
        rotate(rotation);
    }
}
