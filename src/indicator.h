/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef INDICATOR_H
#define INDICATOR_H

#include <QGraphicsItem>
//#include <QGraphicsTextItem>

class Indicator : public QGraphicsItem //public QGraphicsTextItem
{
    //Q_OBJECT
public:
    enum {Type = UserType + 15 };

    Indicator();
    ~Indicator();

    int type() const { return Type; }
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
/*
    QPointF pos();
    QString text();

    QColor bgColor();
    QColor textColor();
  */  
/*
signals:
    void lostFocus(Indicator *item);
    void selectedChange(QGraphicsItem *item);
    
protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    void focusOutEvent(QFocusEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

*/  
private:

};
#endif //INDICATOR_H
