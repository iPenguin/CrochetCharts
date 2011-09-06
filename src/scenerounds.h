/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef SCENEROUNDS_H
#define SCENEROUNDS_H

#include "scene.h"
#include "crochetcell.h"

class QKeyEvent;

class SceneRounds : public Scene
{
    Q_OBJECT
    friend class SaveFile;
    friend class SaveThread;
    friend class SetCellStitch;
    friend class SetCellColor;
    friend class SetCellRotation;
    friend class AddCell;
    friend class RemoveCell;
public:
    
    SceneRounds(QObject* parent = 0);
    ~SceneRounds();

    void createRow(int row, int columns, QString stitch);
    void createChart(int rows, int cols, QString stitch, QSizeF rowSize);

    bool showChartCenter() { return mShowChartCenter; }
    
public slots:
    void setShowChartCenter(bool state);
    
protected:
    /**
     * Takes a @param mousePosition and returns the closest y co-ordinate.
     * function assumes rounds not rows.
     */
    int getClosestRow(QPointF mousePosition);
    /**
     * Takes a @param mousePosition and @param row and returns the closest x co-ordinate.
     * function assumes rounds not rows.
     */
    int getClosestColumn(QPointF mousePosition, int row);

    void setCellPosition(int row, int column, CrochetCell* c, int columns, bool updateAnchor);
    
protected:
    void stitchModeMouseMove(QGraphicsSceneMouseEvent* e);
    void stitchModeMouseRelease(QGraphicsSceneMouseEvent* e);

private:
    QPointF calcPoint(double radius, double angleInDegrees, QPointF origin);

    QGraphicsItem* mCenterSymbol;

    bool mShowChartCenter;

};

#endif //SCENEROUNDS_H
