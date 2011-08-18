/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef CROCHETCELL_H
#define CROCHETCELL_H

#include "cell.h"

class CrochetCell : public Cell
{
    Q_OBJECT
    friend class SaveFile;
public:
    enum {Type = UserType + 10 };
    
    CrochetCell();

    int type() const { return Type; }
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    void setHighlight(bool state) { mHighlight = state; update(); }

    /**
     * The anchor is the original position of the stitch when the chart is created.
     */
    QPointF anchor() { return mAnchor; }

    qreal scale() const { return mScale; }
    void setScale(qreal newScale);

    void setRotation(qreal angle, qreal pivotPoint, bool undo = false);


    qreal origHeight() { return mOrigHeight; }
    qreal origWidth() { return mOrigWidth; }
    
public slots:

    /**
     * The anchor is the original position of the stitch when the chart is created.
     */
    void setAnchor(QPointF a) { mAnchor = a; }
    /**
     * The anchor is the original position of the stitch when the chart is created.
     */
    void setAnchor(qreal x, qreal y) { mAnchor = QPointF(x, y); }

    void setStitch(QString s, bool useAltRenderer);
    void setStitch(Stitch *s, bool useAltRenderer);
    
private:
    qreal mScale;

    qreal mOrigWidth;
    qreal mOrigHeight;

    bool mHighlight;

    QPointF mAnchor;

};

#endif // CROCHETCELL_H
