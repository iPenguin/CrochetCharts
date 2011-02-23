/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef CROCHETSCENE_H
#define CROCHETSCENE_H

#include <QGraphicsScene>

#include "crochetcell.h"

#include <QUndoStack>

class CrochetScene : public QGraphicsScene
{
    Q_OBJECT
    friend class SaveFile;
public:

    enum EditMode {
                StitchMode = 10, //place stitches on the chart.
                ColorMode,       //place colors behind stitches.
                GridMode,        //draw lines on the grid.
                PositionMode     //move the stitches around on the chart.
    };
    
    CrochetScene(QObject *parent = 0);
    ~CrochetScene();
/*
    void setCell(int row, int column, Stitch *s);
    void insertCell(int row, int columnBefore, Stitch *s);

*/
    void createRow(int row, int columns, QString stitch);

    void appendCell(int row, Cell *c, bool fromSave = false);
    void insertCell(int row, int colBefore, Cell *c);

    int rowCount();
    int columnCount(int row);

    Cell* cell(int row, int column);
    //convert x,y to rows, columns.
    Cell* cell(QPoint position);

    void removeCell(int row, int column);
    void createChart(int rows, int cols, QString stitch);

    void setEditMode(EditMode mode) { mMode = mode; }
    EditMode editMode() { return mMode; }

    void setEditStitch(QString stitch) { mEditStitch = stitch; }

    void setEditFgColor(QColor color) { mEditFgColor = color; }
    void setEditBgColor(QColor color) { mEditBgColor = color; }

    QUndoStack* undoStack() { return &mUndoStack; }
    
signals:
    void rowChanged(int row);
    void rowsChanged(int startRow);
    void rowAdded(int newRow);
    void chartCreated(int rows, int cols);
    void stitchChanged(QString oldSt, QString newSt);
    void colorChanged(QString oldColor, QString newColor);
    
protected:
/*
    virtual void    contextMenuEvent ( QGraphicsSceneContextMenuEvent * contextMenuEvent )
    virtual void    helpEvent ( QGraphicsSceneHelpEvent * helpEvent )
    virtual void    keyPressEvent ( QKeyEvent * keyEvent )
    virtual void    keyReleaseEvent ( QKeyEvent * keyEvent )
    virtual void    mouseDoubleClickEvent ( QGraphicsSceneMouseEvent * e )
    virtual void    wheelEvent ( QGraphicsSceneWheelEvent * wheelEvent )
*/
    void mouseMoveEvent(QGraphicsSceneMouseEvent *e);
    void mousePressEvent(QGraphicsSceneMouseEvent *e);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *e);
/*
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
    void dragLeaveEvent(QGraphicsSceneDragDropEvent *event);
    void dragMoveEvent(QGraphicsSceneDragDropEvent *event);
    void dropEvent(QGraphicsSceneDragDropEvent *event);
*/
    //find the x,y positions on the grid for a given cell;
    QPoint findGridPosition(CrochetCell *c);
    
private:
    void stitchModeMouseMove(QGraphicsSceneMouseEvent *e);
    void colorModeMouseMove(QGraphicsSceneMouseEvent *e);
    void gridModeMouseMove(QGraphicsSceneMouseEvent *e);
    void positionModeMouseMove(QGraphicsSceneMouseEvent *e);

    void stitchModeMousePress(QGraphicsSceneMouseEvent *e);
    void colorModeMousePress(QGraphicsSceneMouseEvent *e);
    void gridModeMousePress(QGraphicsSceneMouseEvent *e);
    void positionModeMousePress(QGraphicsSceneMouseEvent *e);

    void stitchModeMouseRelease(QGraphicsSceneMouseEvent *e);
    void colorModeMouseRelease(QGraphicsSceneMouseEvent *e);
    void gridModeMouseRelease(QGraphicsSceneMouseEvent *e);
    void positionModeMouseRelease(QGraphicsSceneMouseEvent *e);

private:
    QPointF calcPoint(double radius, double angleInDegrees, QPointF origin);
    
    void initDemoBackground();

    int mStitchWidth;

    //Used in the mouse*Event()s to keep the mouse movements on the same cell.
    CrochetCell *mCurCell;
    qreal mDiff;

    EditMode mMode;

    QString mEditStitch;
    QColor mEditFgColor;
    QColor mEditBgColor;

    QUndoStack mUndoStack;
    
    //The grid just keeps track of the sts in each row so they can be converted to instructions.
    QList<QList<Cell *> > mGrid;
};

#endif //CROCHETSCENE_H
