/*************************************************\
| (c) 2010-2011 Stitch Works Software             |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "crochetcell.h"

#include <QPainter>
#include <QDebug>

#include <QApplication>
#include <QXmlStreamWriter>

CrochetCell::CrochetCell(const QString fileName) :
    Cell(fileName)
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
    return CrochetCell::Type;
}

void CrochetCell::save(QXmlStreamWriter *stream)
{
    
    stream->writeStartElement("cell"); //start cell
    Cell::save(stream);
    stream->writeEndElement(); //end cell
    
}
