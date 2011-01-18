/*************************************************\
| (c) 2010-2011 Stitch Works Software             |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef CROCHETCELL_H
#define CROCHETCELL_H

#include "cell.h"

class CrochetCell : public Cell
{
    Q_OBJECT
public:
    enum {Type = UserType + 10 };
    
    CrochetCell(const QString fileName);
    ~CrochetCell();

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    int type() const;

    void save(QXmlStreamWriter *stream);
signals:

public slots:

private:


};

#endif // CROCHETCELL_H
