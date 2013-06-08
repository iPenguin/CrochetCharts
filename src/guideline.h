#ifndef GUIDELINE_H
#define GUIDELINE_H

#include <QGraphicsEllipseItem>

class Guideline : public QGraphicsEllipseItem
{
public:
    explicit Guideline(const QRectF &rect, QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);

    enum { Type = UserType + 5 };

    int type () const { return Guideline::Type; }

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
};

#endif // GUIDELINE_H
