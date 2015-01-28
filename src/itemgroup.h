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
#ifndef ITEMGROUP_H
#define ITEMGROUP_H

#include <QGraphicsItemGroup>

class ItemGroup : public QGraphicsItemGroup
{

public:

    enum { Type = UserType + 10 };

    ItemGroup(QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);
    ~ItemGroup();

    int type () const { return ItemGroup::Type; }
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

    bool isGrouped();

    void addToGroup(QGraphicsItem *item);
private:
    QPointF mScale;
};
#endif //ITEMGROUP_H
