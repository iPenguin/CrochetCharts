/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef CROCHETSCENE_H
#define CROCHETSCENE_H

#include <QGraphicsScene>

#include "crochetcell.h"

#include <QUndoStack>
#include <QRubberBand>

class CrochetScene : public QGraphicsScene
{
    Q_OBJECT
    friend class SaveFile;
public:

    enum EditMode {
        StitchMode = 10, //place stitches on the chart.
        ColorMode,       //place colors behind stitches.
        GridMode,        //draw lines on the grid.
        PositionMode,    //move the stitches around on the chart.
        AngleMode        //adjust the angle of the
    };

    enum ChartStyle {
        Flat = 100,
        Round
    };
    
    CrochetScene(QObject *parent = 0);
    ~CrochetScene();

    void createRow(int row, int columns, QString stitch);

    void appendCell(int row, CrochetCell *c, bool fromSave = false);

    //p(x = column, y = row)
    void insertCell(QPoint p, CrochetCell *c);
    
    int rowCount();
    int columnCount(int row);

    //if you have the position in x, y use the overload function
    CrochetCell* cell(int row, int column);
    //convert x,y to rows, columns.
    CrochetCell* cell(QPoint position);

    void removeCell(int row, int column);
    void createChart(CrochetScene::ChartStyle style, int rows, int cols, QString stitch);

    void setEditMode(EditMode mode) { mMode = mode; }
    EditMode editMode() { return mMode; }

    void setEditStitch(QString stitch) { mEditStitch = stitch; }

    void setEditFgColor(QColor color) { mEditFgColor = color; }
    void setEditBgColor(QColor color) { mEditBgColor = color; }

    QUndoStack* undoStack() { return &mUndoStack; }

public slots:
    void updateRubberBand(int dx, int dy);

private slots:
    void stitchUpdated(QString oldSt, QString newSt);

    void updateSelection(QPolygonF selection);
    
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

    void redistributeCells(int row);
    int getClosestRow(QPointF mousePosition);
    int getClosestColumn(QPointF mousePosition);

    qreal scenePosToAngle(QPointF pt);
    
    /**
     * WARING: This funciton should be called after the cell has been added
     * to the grid so that it calcs based on the new count of stitches.
     */
    void setCellPosition(int row, int column, CrochetCell *c, int columns = 1, bool updateAnchor = false);
    
private:
    void highlightCell(QGraphicsSceneMouseEvent *e);
    
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

    void angleModeMouseMove(QGraphicsSceneMouseEvent *e);
    void angleModeMousePress(QGraphicsSceneMouseEvent *e);
    void angleModeMouseRelease(QGraphicsSceneMouseEvent *e);

private:
    QPointF calcPoint(double radius, double angleInDegrees, QPointF origin);
    
    void initDemoBackground();

    int mStitchWidth;

    //Used in the mouse*Event()s to keep the mouse movements on the same cell.
    CrochetCell *mCurCell;
    QSizeF mDiff;
    CrochetCell *mHighlightCell;
    qreal mCurCellRotation;

    QRubberBand *mRubberBand;
    QPointF mRubberBandStart;
    
    int mRowSpacing;
    
    ChartStyle mStyle;
    EditMode mMode;

    bool mFreeForm;
    
    QString mEditStitch;
    QColor mEditFgColor;
    QColor mEditBgColor;

    QUndoStack mUndoStack;
    
    //The grid just keeps track of the sts in each row so they can be converted to instructions.
    QList<QList<CrochetCell *> > mGrid;
};

#endif //CROCHETSCENE_H
