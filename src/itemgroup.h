/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef ITEMGROUP_H
#define ITEMGROUP_H

#include "item.h"
#include <QGraphicsItemGroup>

class ItemGroup : public QGraphicsItemGroup, public Item
{

public:
    
    enum { Type = UserType + 10 };
    
    ItemGroup(QGraphicsItem* parent = 0, QGraphicsScene* scene = 0);
    ~ItemGroup();

    int type () const { return ItemGroup::Type; }
    QRectF boundingRect() const;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);

    bool isGrouped();

    QPointF scale() {return mScale; }
    void setScale(qreal sx, qreal sy);

    void addToGroup(QGraphicsItem* item);
private:
    QPointF mScale;
};
#endif //ITEMGROUP_H
