/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef SCENEBLANK_H
#define SCENEBLANK_H

#include "scene.h"

#include "crochetcell.h"

#include <QUndoStack>
#include <QRubberBand>

#include "indicator.h"

class QKeyEvent;

class SceneBlank : public Scene
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
    
    SceneBlank(QObject *parent = 0);
    ~SceneBlank();

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

    void setCellPosition(int row, int column, CrochetCell *c, int columns = 1, bool updateAnchor = false);
        
private:
    
    void stitchModeMouseMove(QGraphicsSceneMouseEvent *e);
    void stitchModeMousePress(QGraphicsSceneMouseEvent *e);
    void stitchModeMouseRelease(QGraphicsSceneMouseEvent *e);

};

#endif //SCENEBLANK_H
