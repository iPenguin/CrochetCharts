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
    
    SceneRows(QObject* parent = 0);
    ~SceneRows();

    void createRow(int row, int columns, QString stitch);
    void createChart(int rows, int cols, QString stitch, QSizeF rowSize);
        
private:
    
    void stitchModeMouseMove(QGraphicsSceneMouseEvent* e);
    void stitchModeMouseRelease(QGraphicsSceneMouseEvent* e);

};

#endif //SCENEROWS_H
