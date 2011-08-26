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
    
    SceneBlank(QObject* parent = 0);
    ~SceneBlank();

    void createRow(int row, int columns, QString stitch);

    void appendCell(int row, CrochetCell* c);

    //if you have the position in x, y use the overload function
    CrochetCell* cell(int row, int column);
    //convert x,y to rows, columns.
    CrochetCell* cell(QPoint position);

    void removeCell(CrochetCell* c);

    void createChart(int rows, int cols, QString stitch, QSizeF rowSize);
    
signals:
    void stitchChanged(QString oldSt, QString newSt);
    void colorChanged(QString oldColor, QString newColor);
    
protected:

    //find the x,y positions on the grid for a given cell;
    QPoint findGridPosition(CrochetCell* c);
    
    void stitchModeMouseMove(QGraphicsSceneMouseEvent* e);
    void stitchModeMouseRelease(QGraphicsSceneMouseEvent* e);

};

#endif //SCENEBLANK_H
