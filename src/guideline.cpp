#include "guideline.h"

#include <QDebug>
#include <QGraphicsSceneMouseEvent>

Guideline::Guideline(const QRectF &rect, QGraphicsItem *parent, QGraphicsScene *scene) :
    QGraphicsEllipseItem(rect, parent, scene)
{
    setAcceptedMouseButtons(0);
}

void Guideline::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    event->ignore();
}

void Guideline::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    event->ignore();
}

void Guideline::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    event->ignore();
}
