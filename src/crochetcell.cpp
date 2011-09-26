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
#include "settings.h"

CrochetCell::CrochetCell()
    : mHighlight(false)
{
    setFlag(QGraphicsItem::ItemIsMovable);
}

void CrochetCell::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    QColor clr = color();
    if(!clr.isValid())
        clr = QColor(Qt::white);
    
    if(clr != Qt::white)
        painter->fillRect(option->rect, clr);
    if(mHighlight)
        painter->fillRect(option->rect, option->palette.highlight());

    if(stitch()->isSvg()) {
        Cell::paint(painter, option, widget);
    } else {
        painter->drawPixmap(option->rect.x(), option->rect.y(), *(stitch()->renderPixmap()));

         if(option->state & QStyle::State_Selected) {
            painter->setPen(Qt::DashLine);
            painter->drawRect(option->rect);
            painter->setPen(Qt::SolidLine);
        }
    }
}

void CrochetCell::setStitch(QString s, bool useAltRenderer)
{
    Cell::setStitch(s, useAltRenderer);
    setColor(Qt::white);
}

void CrochetCell::setStitch(Stitch* s, bool useAltRenderer)
{
    Cell::setStitch(s, useAltRenderer);
    setColor(Qt::white);
}

CrochetCell* CrochetCell::copy(CrochetCell* cell)
{
    CrochetCell* c = 0;
    if(!cell)
        c = new CrochetCell();
    else
        c = cell;

    c->setStitch(stitch());
    c->setColor(color());
    c->setTransformOriginPoint(transformOriginPoint());
    c->setRotation(rotation());
    c->setScale(scale().x(), scale().y());

    return c;
}
