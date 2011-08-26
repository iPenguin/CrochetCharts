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
    
    SceneRounds(QObject *parent = 0);
    ~SceneRounds();

    void createRow(int row, int columns, QString stitch);

    void appendCell(int row, CrochetCell *c);
    
    //if you have the position in x, y use the overload function
    CrochetCell* cell(int row, int column);
    //convert x,y to rows, columns.
    CrochetCell* cell(QPoint position);

    void removeCell(CrochetCell *c);

    void createChart(int rows, int cols, QString stitch, QSizeF rowSize);

    bool showChartCenter() { return mShowChartCenter; }
    
public slots:
    void setShowChartCenter(bool state);
    
signals:
    void stitchChanged(QString oldSt, QString newSt);
    void colorChanged(QString oldColor, QString newColor);
    
protected:

    /**
     * find the x,y positions on the grid for a given cell;
     */
    QPoint findGridPosition(CrochetCell *c);

    /**
     * Takes a @param row and spread the cells out evenly along it's entire length.
     * This function can handle rounds or rows transparently.
     */
    void redistributeCells(int row);
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
    
    /**
     * WARING: This funciton should be called after the cell has been added
     * to the grid so that it calcs based on the new count of stitches.
     */
    void setCellPosition(int row, int column, CrochetCell *c, int columns = 1, bool updateAnchor = false);
    
protected:
    void stitchModeMouseMove(QGraphicsSceneMouseEvent *e);
    void stitchModeMouseRelease(QGraphicsSceneMouseEvent *e);

private:
    QPointF calcPoint(double radius, double angleInDegrees, QPointF origin);

    QGraphicsItem *mCenterSymbol;

    bool mShowChartCenter;

};

#endif //SCENEROUNDS_H
