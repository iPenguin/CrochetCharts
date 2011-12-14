/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "itemgroup.h"
#include <QGraphicsSceneEvent>

#include "debug.h"

ItemGroup::ItemGroup(QGraphicsItem* parent, QGraphicsScene* scene)
    : QGraphicsItemGroup::QGraphicsItemGroup( parent, scene)
{
}

ItemGroup::~ItemGroup()
{
}


QRectF ItemGroup::boundingRect() const
{
    return QGraphicsItemGroup::boundingRect();
}

void ItemGroup::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    QGraphicsItemGroup::paint(painter, option, widget);
}

void ItemGroup::setScale(qreal sx, qreal sy)
{
    QPointF newScale = QPointF(sx / mScale.x(), sy / mScale.y());

    QGraphicsItemGroup::scale(newScale.x(), newScale.y());
    mScale = QPointF(sx, sy);
}
