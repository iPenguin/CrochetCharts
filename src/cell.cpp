/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "cell.h"

#include <QDebug>
#include "debug.h"

#include <qpainter.h>
#include <qsvgrenderer.h>
#include "stitchlibrary.h"
#include "stitchset.h"
#include "settings.h"
#include <QStyleOption>
#include <QTransform>

Cell::Cell(QGraphicsItem* parent)
    : QGraphicsSvgItem(parent),
    origWidth(64.0),
    origHeight(64.0),
    mStitch(0),
    mScale(QPointF(1.0, 1.0)),
    mHighlight(false)
{
    setCachingEnabled(false);
    setAcceptHoverEvents(true);
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
}

Cell::~Cell()
{
}

QRectF Cell::boundingRect() const
{
    DEBUG("start");
    if(!stitch())
        return QRectF(0,0,0,0);

    if(stitch()->isSvg())
        return QGraphicsSvgItem::boundingRect();
    else
        return stitch()->renderPixmap()->rect();
}

void Cell::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    DEBUG("start");
    if(!stitch())
        return;

    QColor clr = color();
    if(!clr.isValid())
        clr = QColor(Qt::white);

    if(clr != Qt::white)
        painter->fillRect(option->rect, clr);
    if(mHighlight)
        painter->fillRect(option->rect, option->palette.highlight());

    if(stitch()->isSvg()) {
        QGraphicsSvgItem::paint(painter, option, widget);
    } else {
        painter->drawPixmap(option->rect.x(), option->rect.y(), *(stitch()->renderPixmap()));

         if(option->state & QStyle::State_Selected) {
            painter->setPen(Qt::DashLine);
            painter->drawRect(option->rect);
            painter->setPen(Qt::SolidLine);
        }
    }
    DEBUG("end");
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

        origWidth = s->width();
        origHeight = s->height();
        
        emit stitchChanged(old, s->name());
    }
    
    setColor(Qt::white);
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
    QPointF newScale = QPointF(sx / mScale.x(), sy / mScale.y());

    QTransform tx = transform();
    tx.fromScale(newScale.x(), newScale.y());
    setTransform(tx);
    mScale = QPointF(sx, sy);
}

Cell* Cell::copy(Cell* cell)
{
    Cell* c = 0;
    if(!cell)
        c = new Cell();
    else
        c = cell;

    c->setStitch(stitch());
    c->setColor(color());
    c->setTransformOriginPoint(transformOriginPoint());
    c->setRotation(rotation());
    c->setScale(scale().x(), scale().y());

    return c;
}
