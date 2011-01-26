/*************************************************\
| (c) 2010-2011 Stitch Works Software             |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "cell.h"

#include <QDebug>
#include <qpainter.h>
#include <qsvgrenderer.h>
#include "stitchcollection.h"
#include "stitchset.h"

Cell::Cell(QGraphicsItem *parent) :
    QGraphicsSvgItem(parent), mStitch(0)
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
    //painter->setPen(Qt::red);
    //renderer()->render(painter, boundingRect());  
    QGraphicsSvgItem::paint(painter, option, widget);
}

void Cell::setStitch(Stitch* s)
{
    if(mStitch != s) {
        Stitch* old = mStitch;
        mStitch = s;
        setSharedRenderer(s->renderSvg());
        emit stitchChanged(old, s);
    }
}

void Cell::setStitch(QString s)
{
    Stitch* stitch = 0;

    stitch = StitchCollection::inst()->masterStitchSet()->findStitch(s);

    //FIXME: do a look up for the default stitch.
    if(!stitch)
        stitch = StitchCollection::inst()->masterStitchSet()->findStitch("ch");

    setStitch(stitch);
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

QString Cell::name()
{
    if(!mStitch)
        return QString();
    else
        return mStitch->name();
}


