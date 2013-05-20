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

Cell::Cell(QGraphicsItem *parent)
    : QGraphicsSvgItem(parent),
    mStitch(0),
    mScale(QPointF(1.0, 1.0)),
    mHighlight(false)
{
    setCachingEnabled(false);
    setAcceptHoverEvents(true);
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);

    //if we don't set the bgColor it'll end up black.
    setBgColor();
}

Cell::~Cell()
{
}

QRectF Cell::boundingRect() const
{
    if(!stitch())
        return QRectF(0,0,0,0);

    if(stitch()->isSvg())
        return QGraphicsSvgItem::boundingRect();
    else
        return stitch()->renderPixmap()->rect();
}

void Cell::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{

    if(!stitch())
        return;

    QColor clr = bgColor();
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
}

bool Cell::isGrouped()
{

    if(parentItem()) {
        qDebug() << "isGrouped parent type:" << parentItem()->Type;
        return true;
    }

    return false;
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
        if(s->isSvg()) {
            setSharedRenderer(s->renderSvg(mColor));
        }

        if(doUpdate)
            update();
        
        emit stitchChanged(old, s->name());
    }
    
    setTransformOriginPoint(s->width()/2, s->height());
}

void Cell::setBgColor(QColor c)
{
    if (mBgColor != c) {
        QString old = "";
        if (mBgColor.isValid())
            old = mBgColor.name();
        mBgColor = c;
        emit bgColorChanged(old, c.name());
        update();
    }
}

void Cell::setColor(QColor c)
{

    if(mColor != c) {
        QString old = "";
        if(mColor.isValid())
            old = mColor.name();
        mColor = c;
        setSharedRenderer(stitch()->renderSvg(c));

        emit colorChanged(old, c.name());
        update();
    }
}

void Cell::setStitch(QString s)
{
    Stitch *stitch = StitchLibrary::inst()->findStitch(s);

    if (!stitch) {
        QString st = Settings::inst()->value("defaultStitch").toString();
        stitch = StitchLibrary::inst()->findStitch(st);
    }

    setStitch(stitch);
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
    if(mStitch->isSvg() && mStitch->renderSvg()->isValid()) {
        QString color = "#000000";
        if(useAlt) {
            color = Settings::inst()->value("stitchAlternateColor").toString();
        }
        setSharedRenderer(mStitch->renderSvg(color));
    }
}

void Cell::setScale(qreal sx, qreal sy)
{

    QPointF newScale = QPointF(sx / mScale.x(), sy / mScale.y());

    QGraphicsSvgItem::setTransform(transform().scale(newScale.x(), newScale.y()));

    mScale = QPointF(sx, sy);
}

Cell* Cell::copy(Cell *cell)
{
    Cell *c = 0;
    if(!cell)
        c = new Cell();
    else
        c = cell;

    c->setStitch(stitch());
    c->setBgColor(bgColor());
    c->setColor(c->color());
    c->setTransformOriginPoint(transformOriginPoint());
    c->setRotation(rotation());
    c->setScale(scale().x(), scale().y());

    return c;
}
