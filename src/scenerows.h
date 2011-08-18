/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef SCENEROWS_H
#define SCENEROWS_H

#include "scene.h"

#include "crochetcell.h"

#include <QUndoStack>
#include <QRubberBand>

#include "indicator.h"

class QKeyEvent;

class SceneRows : public Scene
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
    
    SceneRows(QObject *parent = 0);
    ~SceneRows();

    void createRow(int row, int columns, QString stitch);

    void appendCell(int row, CrochetCell *c, bool fromSave = false);

    /**
     * p(x = column, y = row)
     */
    void addCell(QPoint p, CrochetCell *c);
    
    int rowCount();
    int columnCount(int row);

    //if you have the position in x, y use the overload function
    CrochetCell* cell(int row, int column);
    //convert x,y to rows, columns.
    CrochetCell* cell(QPoint position);

    void removeCell(CrochetCell *c);

    void createChart(int rows, int cols, QString stitch, QSizeF rowSize);
    
signals:
    void stitchChanged(QString oldSt, QString newSt);
    void colorChanged(QString oldColor, QString newColor);
    
protected:

    void keyReleaseEvent(QKeyEvent *keyEvent);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *e);
    void mousePressEvent(QGraphicsSceneMouseEvent *e);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *e);

    //find the x,y positions on the grid for a given cell;
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

    qreal scenePosToAngle(QPointF pt);
    
    /**
     * WARING: This funciton should be called after the cell has been added
     * to the grid so that it calcs based on the new count of stitches.
     */
    void setCellPosition(int row, int column, CrochetCell *c, int columns = 1, bool updateAnchor = false);
    
private:
    
    void stitchModeMouseMove(QGraphicsSceneMouseEvent *e);
    void stitchModeMouseRelease(QGraphicsSceneMouseEvent *e);
    
    void angleModeMouseMove(QGraphicsSceneMouseEvent *e);
    void angleModeMousePress(QGraphicsSceneMouseEvent *e);
    void angleModeMouseRelease(QGraphicsSceneMouseEvent *e);

    void stretchModeMouseMove(QGraphicsSceneMouseEvent *e);
    void stretchModeMouseRelease(QGraphicsSceneMouseEvent *e);

private:
    QPointF calcPoint(double radius, double angleInDegrees, QPointF origin);

    qreal mScale;
    qreal mOldScale;

    qreal mAngleDelta;
};

#endif //SCENEROWS_H
