/*************************************************\
| (c) 2010-2011 Stitch Works Software             |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "cell.h"

#include <QXmlStreamWriter>

#include <QDebug>

Cell::Cell(const QString fileName, QGraphicsItem *parent) :
    QGraphicsSvgItem(fileName, parent)
{
//    this->setCacheMode(QGraphicsItem::ItemCoordinateCache, QSize(32,64));

}

Cell::Cell(QGraphicsItem *parent) :
    QGraphicsSvgItem(parent)
{
//    this->setCacheMode(QGraphicsItem::ItemCoordinateCache, QSize(32,64));
}

QRectF Cell::boundingRect() const
{
    return QGraphicsSvgItem::boundingRect();
}

void Cell::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QGraphicsSvgItem::paint(painter, option, widget);
}

int Cell::type() const
{
    return Cell::Type;
}

void Cell::save(QXmlStreamWriter *stream)
{
    stream->writeTextElement("x", QString::number(pos().x()));
    stream->writeTextElement("y", QString::number(pos().y()));
    stream->writeTextElement("rotation", QString::number(mRotation));
    stream->writeTextElement("angle", QString::number(mAngle));
}
