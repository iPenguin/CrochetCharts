/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "itemgroup.h"
#include <QGraphicsSceneEvent>

#include "debug.h"

ItemGroup::ItemGroup(QGraphicsItem *parent, QGraphicsScene *scene)
    : QGraphicsItemGroup( parent, scene),
    mScale(QPointF(1.0, 1.0))
{

    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
}

ItemGroup::~ItemGroup()
{
}

QRectF ItemGroup::boundingRect() const
{
    return QGraphicsItemGroup::boundingRect();
}

void ItemGroup::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{

    QGraphicsItemGroup::paint(painter, option, widget);
}

bool ItemGroup::isGrouped()
{
    if(parentItem()) {
        if(parentItem()->Type == ItemGroup::Type) {
            return true;
        }
    }

    return false;
}

void ItemGroup::addToGroup(QGraphicsItem *item)
{

    QGraphicsItemGroup::addToGroup(item);

}
