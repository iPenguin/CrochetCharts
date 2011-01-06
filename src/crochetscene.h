/*************************************************\
| (c) 2010-2011 Stitch Works Software             |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef CROCHETSCENE_H
#define CROCHETSCENE_H

#include <QGraphicsScene>

class CrochetScene : public QGraphicsScene
{
    Q_OBJECT
public:
    CrochetScene(QObject *parent = 0);
/*
    void setCell(int row, int column, Stitch *s);
    void insertCell(int row, int columnBefore, Stitch *s);

*/
    void createRow(int columns);
private:
    QPointF calcPoint(double radius, double angleInDegrees, QPointF origin);

    void initDemoBackground();

    int mStitchWidth;
};

#endif //CROCHETSCENE_H
