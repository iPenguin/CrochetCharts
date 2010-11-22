#include "cell.h"

Cell::Cell(const QString fileName, QGraphicsItem *parent) :
    QGraphicsSvgItem(fileName, parent)
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

int Cell::type () const
{
    return 0;
}
