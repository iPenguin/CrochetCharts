/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "cell.h"

#include <QDebug>
#include <qpainter.h>
#include <qsvgrenderer.h>
#include "stitchcollection.h"
#include "stitchset.h"
#include "settings.h"
#include <QStyleOption>

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
    if(stitch()->isSvg())
        return QGraphicsSvgItem::boundingRect();
    else
        return stitch()->renderPixmap()->rect();
}

void Cell::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QGraphicsSvgItem::paint(painter, option, widget);
}

void Cell::setStitch(Stitch *s)
{
    if (mStitch != s) {
        QString old;
        bool doUpdate = false;
        
        if (mStitch) {
            old = mStitch->name();
            doUpdate = (mStitch->isSvg() != s->isSvg());
        }
        mStitch = s;
        if(s->isSvg() && s->renderSvg()->isValid())
            setSharedRenderer(s->renderSvg());

        if(doUpdate)
            update();
            
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

    stitch = StitchCollection::inst()->findStitch(s);

    if (!stitch) {
        QString st = Settings::inst()->value("defaultStitch").toString();
        stitch = StitchCollection::inst()->findStitch(st);
    }

    setStitch(stitch);
}

int Cell::type() const
{
    return Cell::Type;
}

QString Cell::name()
{
    if (!mStitch)
        return QString();
    else
        return mStitch->name();
}


