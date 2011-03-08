/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "cell.h"

#include <QDebug>
#include <qpainter.h>
#include <qsvgrenderer.h>
#include "stitchlibrary.h"
#include "stitchset.h"
#include "settings.h"
#include <QStyleOption>

Cell::Cell(QGraphicsItem *parent) :
        QGraphicsSvgItem(parent), mStitch(0)
{
    this->setCachingEnabled(false);
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

void Cell::setStitch(Stitch* s, bool useAltRenderer)
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
            setSharedRenderer(s->renderSvg(useAltRenderer));

        if(doUpdate)
            update();
            
        emit stitchChanged(old, s->name());
    }
}

void Cell::setColor(QColor c)
{
    if (mColor != c) {
        QString old = "";
        if (mColor.isValid())
            old = mColor.name();
        mColor = c;
        emit colorChanged(old, c.name());
        update();
    }
}

void Cell::setStitch(QString s, bool useAltRenderer)
{
    Stitch* stitch = 0;

    stitch = StitchLibrary::inst()->findStitch(s);

    if (!stitch) {
        QString st = Settings::inst()->value("defaultStitch").toString();
        stitch = StitchLibrary::inst()->findStitch(st);
    }

    setStitch(stitch, useAltRenderer);
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


