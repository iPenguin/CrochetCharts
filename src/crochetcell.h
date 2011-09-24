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

    CrochetCell* copy(CrochetCell* cell = 0);
    
public slots:

    void setStitch(QString s, bool useAltRenderer = false);
    void setStitch(Stitch* s, bool useAltRenderer = false);
    
private:
    bool mHighlight;

};

#endif // CROCHETCELL_H
