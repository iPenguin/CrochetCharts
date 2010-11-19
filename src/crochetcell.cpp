#include "crochetcell.h"

CrochetCell::CrochetCell(QGraphicsItem *parent) :
    Cell(parent)
{

}

CrochetCell::~CrochetCell()
{

}

QRectF CrochetCell::boundingRect () const
{

    return QRectF();
}

void CrochetCell::paint(QPainter */*painter*/, const QStyleOptionGraphicsItem */*option*/, QWidget */*widget*/)
{

}

int CrochetCell::type () const
{
    return 0;
}
