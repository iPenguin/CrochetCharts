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
     : mScale(1.0), mHighlight(false)
{
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges); //enable itemChange to pick up move changes.
}

void CrochetCell::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if(color() != Qt::white)
        painter->fillRect(option->rect, color());
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
/*
    QRect rect =QRect(option->rect.x() + (option->rect.width()/2), option->rect.y() + (option->rect.height()/2),
                      (option->rect.width()/2), (option->rect.height()/2));
*/
}

void CrochetCell::setScale(qreal newScale)
{
    qreal newSize = mOrigHeight * newScale;

    qreal scale = newSize/mOrigHeight;
    QTransform trans =  transform().scale(1, scale);
    setTransform(trans);

    mScale = newScale;
}

void CrochetCell::setStitch(QString s, bool useAltRenderer)
{
    Cell::setStitch(s, useAltRenderer);
    mOrigWidth = boundingRect().width();
    mOrigHeight = boundingRect().height();
}

void CrochetCell::setStitch(Stitch *s, bool useAltRenderer)
{
   Cell::setStitch(s, useAltRenderer);

   mOrigWidth = boundingRect().width();
   mOrigHeight = boundingRect().height();
}

void CrochetCell::setRotation(qreal angle, qreal pivotPoint, bool undo)
{
    
    setTransform(QTransform().translate(pivotPoint, 0).rotate(angle).translate(-pivotPoint, 0));
    if(!undo)
        setScale(mScale);
    setAngle(angle);
    
}
