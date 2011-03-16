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

    QPointF anchor() { return mAnchor; }
signals:

public slots:
    void setAnchor(QPointF a) { mAnchor = a; }
    void setAnchor(qreal x, qreal y) { mAnchor = QPointF(x, y); }

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    
private:
    bool mHighlight;

    QPointF mAnchor;

};

#endif // CROCHETCELL_H
