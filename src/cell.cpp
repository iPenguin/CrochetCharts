#include "cell.h"

Cell::Cell(QGraphicsItem *parent) :
    QGraphicsSvgItem(parent)
{
}

QRectF Cell::boundingRect () const
{
    return QGraphicsSvgItem::boundingRect();
}

void Cell::paint(QPainter */*painter*/, const QStyleOptionGraphicsItem */*option*/, QWidget */*widget*/)
{

}

int Cell::type () const
{
    return 0;
}
