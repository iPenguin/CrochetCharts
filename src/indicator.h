/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef INDICATOR_H
#define INDICATOR_H

#include <QGraphicsItem>

class Indicator : public QGraphicsItem
{
    
public:
    enum {Type = UserType + 15 };

    Indicator();
    ~Indicator();

    int type() const { return Type; }
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:

};
#endif //INDICATOR_H
