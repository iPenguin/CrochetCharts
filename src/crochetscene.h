/*************************************************\
| (c) 2010-2011 Stitch Works Software             |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef CROCHETSCENE_H
#define CROCHETSCENE_H

#include <QGraphicsScene>

#include "crochetdatamodel.h"

class CrochetScene : public QGraphicsScene
{
    Q_OBJECT
public:
    CrochetScene(QObject *parent = 0);
/*
    void setCell(int row, int column, Stitch *s);
    void insertCell(int row, int columnBefore, Stitch *s);

*/
    void createRow(int row, int columns);
private:
    QPointF calcPoint(double radius, double angleInDegrees, QPointF origin);

    void initDemoBackground();

    int mStitchWidth;

    CrochetDataModel *mModel;
};

#endif //CROCHETSCENE_H
