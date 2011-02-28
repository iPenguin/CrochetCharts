/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef LEGENDS_H
#define LEGENDS_H

#include <QGraphicsWidget>

class ColorLegend : public QGraphicsWidget
{
    Q_OBJECT
public:
    ColorLegend(QGraphicsItem *parent = 0, Qt::WindowFlags wFlags = 0);
    ~ColorLegend();

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

private:

};
#endif //LEGENDS_H