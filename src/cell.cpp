/*************************************************\
| (c) 2010 Stitch Works Software                  |
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
    if (mStitch != s) {
        QString old;
        if (mStitch)
            old = mStitch->name();
        mStitch = s;
        setSharedRenderer(s->renderSvg());
        emit stitchChanged(old, s->name());
    }
}

void Cell::setBgColor(QColor c)
{
    if (mBgColor != c) {
        QString old = "";
        if (mBgColor.isValid())
            old = mBgColor.name();
        mBgColor = c;
        emit colorChanged(old, c.name());
    }
}

void Cell::setStitch(QString s)
{
    Stitch* stitch = 0;

    stitch = StitchCollection::inst()->masterStitchSet()->findStitch(s);

    //FIXME: do a look up for the default stitch.
    if (!stitch)
        stitch = StitchCollection::inst()->masterStitchSet()->findStitch("ch");

    setStitch(stitch);
}

int Cell::type() const
{
    return Cell::Type;
}

void Cell::setRotation(qreal rotation)
{
    if (mRotation != rotation) {
        mRotation = rotation;
        rotate(rotation);
    }
}

QString Cell::name()
{
    if (!mStitch)
        return QString();
    else
        return mStitch->name();
}


