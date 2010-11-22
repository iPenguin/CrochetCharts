#include "crochetcell.h"

#include <QPainter>

CrochetCell::CrochetCell(const QString fileName, QGraphicsItem *parent) :
    Cell(fileName, parent)
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
}

int CrochetCell::type () const
{
    return 0;
}
