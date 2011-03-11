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
    ~CrochetCell();

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

    int type() const { return Type; }
    GraphicsItemFlags flags() const;

    void setHighlight(bool state) { mHighlight = state; update(); }

signals:

public slots:

private:
    bool mHighlight;

};

#endif // CROCHETCELL_H
