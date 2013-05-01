/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef ITEM_H
#define ITEM_H

#include <QPointF>

class Item
{

public:

    virtual QPointF scale() = 0;
    virtual void setScale(qreal sx, qreal sy) = 0;

    virtual bool isGrouped() = 0;

    qreal origWidth;
    qreal origHeight;

private:

};
#endif //ITEM_H
