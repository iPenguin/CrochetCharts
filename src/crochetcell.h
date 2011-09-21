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
    friend class SaveThread;
public:
    enum {Type = UserType + 10 };
    
    CrochetCell();

    int type() const { return Type; }
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);
    void setHighlight(bool state) { mHighlight = state; update(); }

    qreal scale() const { return mScale; }
    void setScale(qreal newScale, QPointF pivotPoint);

    void setRotation(qreal angle, QPointF pivotPoint);


    qreal origHeight() { return mOrigHeight; }
    qreal origWidth() { return mOrigWidth; }

    CrochetCell* copy(CrochetCell* cell = 0);

    void setAngle(qreal angle);
    
public slots:

    void setStitch(QString s, bool useAltRenderer = false);
    void setStitch(Stitch* s, bool useAltRenderer = false);
    
private:
    qreal mScale;

    qreal mOrigWidth;
    qreal mOrigHeight;

    bool mHighlight;

};

#endif // CROCHETCELL_H
