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

Cell::Cell(QGraphicsItem* parent)
    : QGraphicsSvgItem(parent),
    mStitch(0),
    mScale(QPointF(1.0, 1.0)),
    mActualScale(QPointF(1.0, 1.0))
{
    setCachingEnabled(false);
    setAcceptHoverEvents(true);
    setFlag(QGraphicsItem::ItemIsSelectable);
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

void Cell::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
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
    Stitch* stitch = StitchLibrary::inst()->findStitch(s);

    if (!stitch) {
        QString st = Settings::inst()->value("defaultStitch").toString();
        stitch = StitchLibrary::inst()->findStitch(st);
    }

    setStitch(stitch, useAltRenderer);
}

QString Cell::name()
{
    if(mStitch)
        return mStitch->name();
    else
        return QString();
}

void Cell::useAlternateRenderer(bool useAlt)
{
    if(mStitch->isSvg() && mStitch->renderSvg()->isValid())
        setSharedRenderer(mStitch->renderSvg(useAlt));
}

void Cell::setScale(qreal sx, qreal sy)
{

    mActualScale.setX(mActualScale.x()*sx);
    mActualScale.setY(mActualScale.y()*sy);
    
    mScale = QPointF(sx, sy);
    QGraphicsSvgItem::scale(sx, sy);
}

void Cell::setActualScale(qreal sx, qreal sy)
{
    QPointF newScale = QPointF(sx / mScale.x(), sy / mScale.y());

    setScale(newScale.x(), newScale.y());
}
