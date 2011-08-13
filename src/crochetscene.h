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

#include "indicator.h"

class QKeyEvent;

class CrochetScene : public QGraphicsScene
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

    enum EditMode {
        StitchMode = 10, //place stitches on the chart.
        ColorMode,       //place colors behind stitches.
        GridMode,        //draw lines on the grid.
        PositionMode,    //move the stitches around on the chart.
        AngleMode,       //adjust the angle of the
        StretchMode,       //stretch the stitches.
        IndicatorMode
    };

    enum ChartStyle {
        Rows = 100,
        Rounds
    };
    
    CrochetScene(QObject *parent = 0);
    ~CrochetScene();

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

    void createChart(CrochetScene::ChartStyle style, int rows, int cols, QString stitch, QSizeF rowSize);

    void setEditMode(EditMode mode) { mMode = mode; }
    EditMode editMode() { return mMode; }

    void setEditStitch(QString stitch) { mEditStitch = stitch; }

    void setEditFgColor(QColor color) { mEditFgColor = color; }
    void setEditBgColor(QColor color) { mEditBgColor = color; }

    QUndoStack* undoStack() { return &mUndoStack; }

    void addIndicator(Indicator *i);
    void removeIndicator(Indicator *i);

    QList<QList<CrochetCell *> > grid() { return mGrid; }

    bool showChartCenter() { return mShowChartCenter; }
    
    CrochetScene::ChartStyle chartStyle() const { return mStyle; }
    
public slots:
    void setShowChartCenter(bool state);
    
    void updateRubberBand(int dx, int dy);

private slots:
    void stitchUpdated(QString oldSt, QString newSt);

    void updateSelection(QPolygonF selection);
    
signals:
    void stitchChanged(QString oldSt, QString newSt);
    void colorChanged(QString oldColor, QString newColor);
    
protected:
/*
    virtual void    contextMenuEvent ( QGraphicsSceneContextMenuEvent * contextMenuEvent )
    virtual void    helpEvent ( QGraphicsSceneHelpEvent * helpEvent )
    virtual void    keyPressEvent ( QKeyEvent * keyEvent )
    virtual void    wheelEvent ( QGraphicsSceneWheelEvent * wheelEvent )
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *e);
*/
    void keyReleaseEvent(QKeyEvent *keyEvent);
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

    QList<Indicator*> indicators() { return mIndicators; }

    qreal scenePosToAngle(QPointF pt);
    
    /**
     * WARING: This funciton should be called after the cell has been added
     * to the grid so that it calcs based on the new count of stitches.
     */
    void setCellPosition(int row, int column, CrochetCell *c, int columns = 1, bool updateAnchor = false);
    
private:
    void highlightCell(QGraphicsSceneMouseEvent *e);
    
    void stitchModeMouseMove(QGraphicsSceneMouseEvent *e);
    void stitchModeMousePress(QGraphicsSceneMouseEvent *e);
    void stitchModeMouseRelease(QGraphicsSceneMouseEvent *e);
    
    void colorModeMouseMove(QGraphicsSceneMouseEvent *e);
    void colorModeMousePress(QGraphicsSceneMouseEvent *e);
    void colorModeMouseRelease(QGraphicsSceneMouseEvent *e);
    
    void positionModeMouseMove(QGraphicsSceneMouseEvent *e);
    void positionModeMousePress(QGraphicsSceneMouseEvent *e);
    void positionModeMouseRelease(QGraphicsSceneMouseEvent *e);

    void angleModeMouseMove(QGraphicsSceneMouseEvent *e);
    void angleModeMousePress(QGraphicsSceneMouseEvent *e);
    void angleModeMouseRelease(QGraphicsSceneMouseEvent *e);

    void stretchModeMouseMove(QGraphicsSceneMouseEvent *e);
    void stretchModeMousePress(QGraphicsSceneMouseEvent *e);
    void stretchModeMouseRelease(QGraphicsSceneMouseEvent *e);

    void indicatorModeMouseMove(QGraphicsSceneMouseEvent *e);
    void indicatorModeMousePress(QGraphicsSceneMouseEvent *e);
    void indicatorModeMouseRelease(QGraphicsSceneMouseEvent *e);

private:
    QPointF calcPoint(double radius, double angleInDegrees, QPointF origin);
    
    void initDemoBackground();

    /**
     * Used in the mouse*Event()s to keep the mouse movements on the same cell.
     */
    CrochetCell *mCurCell;
    QPointF mCellStartPos;
    QPointF mLeftButtonDownPos;
    
    Indicator *mCurIndicator;
    
    /**
     * The difference between where the user clicked on the object and the (x,y) of the object.
     */
    QSizeF mDiff;
    CrochetCell *mHighlightCell;
    qreal mCurCellRotation;

    QRubberBand *mRubberBand;
    QPointF mRubberBandStart;

    QMap<QGraphicsItem *, QPointF> mOldPositions;
    
    //Is the user moving an indicator.
    bool mMoving;

    int mRowSpacing;
    
    ChartStyle mStyle;
    EditMode mMode;

    QGraphicsItem *mCenterSymbol;
    
    QString mEditStitch;
    QColor mEditFgColor;
    QColor mEditBgColor;

    bool mShowChartCenter;
    
    QUndoStack mUndoStack;

    QSizeF mDefaultSize;
    
    //The grid just keeps track of the sts in each row so they can be converted to instructions.
    QList<QList<CrochetCell *> > mGrid;

    QList<Indicator*> mIndicators;
};

#endif //CROCHETSCENE_H
