/****************************************************************************\
 Copyright (c) 2011-2014 Stitch Works Software
 Brian C. Milco <bcmilco@gmail.com>

 This file is part of Crochet Charts.

 Crochet Charts is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Crochet Charts is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Crochet Charts. If not, see <http://www.gnu.org/licenses/>.

 \****************************************************************************/
#include "itemgroup.h"
#include <QPainter>
#include <QGraphicsSceneEvent>

#include "debug.h"

ItemGroup::ItemGroup(QGraphicsItem *parent, QGraphicsScene *scene)
    : QGraphicsItemGroup( parent, scene),
    mScale(QPointF(1.0, 1.0))
{
    setTransform(QTransform(1,0,0,0,1,0,0,0,1));
    setPos(0, 0);
	setTransformOriginPoint(0, 0);
	setRotation(0);

    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setHandlesChildEvents(true);
}

ItemGroup::~ItemGroup()
{
}

QRectF ItemGroup::boundingRect() const
{
	QRectF bb = QGraphicsItemGroup::boundingRect();
    return bb;
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
