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
#include "crochetscene.h"

CrochetCell::CrochetCell()
     : mScale(1.0), mHighlight(false)
{
    //setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges); //enable itemChange to pick up move changes.
}

void CrochetCell::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if(color() != Qt::white)
        painter->fillRect(option->rect, color());
    if(mHighlight)
        painter->fillRect(option->rect, option->palette.highlight());
    
    if(stitch()->isSvg())
        Cell::paint(painter, option, widget);
    else
        painter->drawPixmap(option->rect.x(), option->rect.y(), *(stitch()->renderPixmap()));
    
    QRect rect =QRect(option->rect.x() + (option->rect.width()/2), option->rect.y() + (option->rect.height()/2),
                      (option->rect.width()/2), (option->rect.height()/2));
}

QVariant CrochetCell::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if(change == ItemPositionChange && scene()) {
        QPointF newPos = value.toPointF();
        qreal x, y;

        //FIXME: I really don't like having to do cast to CrochetScene in the CrochetCell!
        CrochetScene *s = static_cast<CrochetScene*>(scene());
        
        bool freeForm = s->isFreeForm();
        if(!freeForm && !mAnchor.isNull()) {
            x = qMin(mAnchor.x() + 64, qMax(newPos.x(), mAnchor.x() - 64));
            y = qMin(mAnchor.y() + 64, qMax(newPos.y(), mAnchor.y() - 64));
        } else {
            x = newPos.x();
            y = newPos.y();
        }
        return QVariant(QPointF(x, y));
    }
    
    return QGraphicsItem::itemChange(change, value);

}

void CrochetCell::setScale(qreal newScale)
{
    qDebug() << "cell setScale" << newScale;
    if(newScale < 0.1)
        newScale = 0.1;
    
    if(newScale != mScale) {
        mScale = newScale;
        QTransform trans =  transform().scale(1, newScale);
        setTransform(trans);
    }
}
