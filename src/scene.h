/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef SCENE_H
#define SCENE_H

#include <QGraphicsScene>

#include "cell.h"

#include <QUndoStack>
#include <QRubberBand>

#include "indicator.h"

class QKeyEvent;

class Scene : public QGraphicsScene
{
    Q_OBJECT
    friend class SaveFile;
    friend class SaveThread;
    friend class RowEditDialog;
    friend class TextView;

public:

    enum EditMode {
        StitchEdit = 10, //place stitches on the chart.
        ColorEdit = 11,       //place colors behind stitches.
        RowEdit = 12,
        RotationEdit = 14,       //adjust the angle of the
        ScaleEdit = 15,       //scale the stitches.
        IndicatorEdit = 16
    };

    enum ChartStyle {
        Rows = 100, // flat chart rows calc'ed based on grid.
        Rounds,     // round chart rows clac'ed with trig.
        Blank       // start w/no sts and allow anything.
    };
    
    Scene(QObject* parent = 0);
    ~Scene();

    //if you have the position in x, y use the overload function
    Cell* cell(int row, int column);
    //convert x,y to rows, columns.
    Cell* cell(QPoint position);

    /**
     * find the x,y positions on the grid for a given cell;
     * return QPoint(column, row);
     * if return = -1,-1 isVoid.
     */
    QPoint indexOf(Cell* c);
    
    int rowCount();
    int columnCount(int row);
    int maxColumnCount();

    void setEditMode(EditMode mode);
    EditMode editMode() { return mMode; }

    void setEditStitch(QString stitch) { mEditStitch = stitch; }

    void setEditFgColor(QColor color) { mEditFgColor = color; }
    void setEditBgColor(QColor color) { mEditBgColor = color; }

    QUndoStack* undoStack() { return &mUndoStack; }
    
    QStringList modes();

    void moveRowUp(int row);
    void moveRowDown(int row);

    void removeRow(int row);

    void alignSelection(int alignmentStyle);
    void distributeSelection(int distributionStyle);
    void arrangeGrid(QSize grid, QSize alignment, QSize spacing, bool useSelection);
    
    /**
     * directions: left = 1
     *             right = 2
     *             up = 3
     *             down = 4
     */
    void mirror(int direction);
    void rotate(qreal degrees);

    void group();
    void ungroup();

    void createRowsChart(int rows, int cols, QString defStitch, QSizeF rowSize);
    void createBlankChart();

    void addItem(QGraphicsItem* item);
    void removeItem(QGraphicsItem* item);

    /**
     * Add a row of stitches to the grid.
     * If append == false, use the rowPos to insert the row into the grid.
     */
    void gridAddRow(QList< Cell* > row, bool append = true, int before = 0);

public slots:    
    void copy();
    void cut();
    void paste();
    
protected:
    void copyRecursively(QDataStream &stream, QList<QGraphicsItem*> items);
    void pasteRecursively(QDataStream &stream, QList<QGraphicsItem*> *group);

    /**
     * This function removes a cell from the 'grid'. if the row is empty it removes the row too.
     */
    void removeFromRows(Cell* c);

    void updateSceneRect();
    
public slots:
    void createRow();
    void updateRow(int row);
    
    /**
     * highlight (select) all the stitches in row @param row.
     */
    void highlightRow(int row);
    void drawRowLines(int row);

    void highlightIndicators(bool state);

    void updateRubberBand(int dx, int dy);

    void editorLostFocus(Indicator *item);
    void editorGotFocus(Indicator *item);
    
signals:
    void stitchChanged(QString oldSt, QString newSt);
    void colorChanged(QString oldColor, QString newColor);

    void rowSelected();

    //When ever a row is edited emit this signal.
    void rowEdited(bool state);
    
protected:
//    virtual void    helpEvent ( QGraphicsSceneHelpEvent * helpEvent )
    void contextMenuEvent(QGraphicsSceneContextMenuEvent* e);

    void keyReleaseEvent(QKeyEvent* keyEvent);
    void keyPressEvent(QKeyEvent* keyEvent);
    
    void stitchModeKeyRelease(QKeyEvent* keyEvent);
    void angleModeKeyRelease(QKeyEvent* keyEvent);
    void scaleModeKeyRelease(QKeyEvent* keyEvent);
    
    void mouseMoveEvent(QGraphicsSceneMouseEvent* e);
    void mousePressEvent(QGraphicsSceneMouseEvent* e);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* e);
    
    QList<Indicator*> indicators() { return mIndicators; }
        
    void initDemoBackground();

    /**
     * vertial:    0 - don't change alignment
     *             1 - align top
     *             2 - align center v
     *             3 - align bottom
     * horizontal: 0 - don't change alignment
     *             1 - align left
     *             2 - align center h
     *             3 - align right
     */
    void align(int vertical, int horizontal);
    /**
     * vertial:    0 - don't change distribution
     *             1 - distribute top
     *             2 - distribute center v
     *             3 - distribute bottom
     * horizontal: 0 - don't change distribution
     *             1 - distribute left
     *             2 - distribute center h
     *             3 - distribute right
     */
    void distribute(int vertical, int horizontal);
    
    void alignToPath();
    void distributeToPath();

    /**
     * Because of the fact that the group doesn't return the correct scene co-ordinates
     * this function will correct them.
     */
    QPointF calcGroupPos(QGraphicsItem* group, QPointF newScenePos);

public:
    QGraphicsItemGroup* group(QList<QGraphicsItem*> items, QGraphicsItemGroup* g = 0);
    void ungroup(QGraphicsItemGroup* group);

    QRectF selectedItemsBoundingRect(QList<QGraphicsItem*> items);

    void rotateSelection(qreal degrees, QList<QGraphicsItem*> items, QPointF pivotPoint);

    /**
     * This function overrides the itemsBoundingRect().
     */
    QRectF itemsBoundingRect();
    
protected:
    void colorModeMouseMove(QGraphicsSceneMouseEvent* e);
    void colorModeMouseRelease(QGraphicsSceneMouseEvent* e);
    
    void indicatorModeMouseMove(QGraphicsSceneMouseEvent* e);
    void indicatorModeMouseRelease(QGraphicsSceneMouseEvent* e);

    void angleModeMousePress(QGraphicsSceneMouseEvent* e);
    void angleModeMouseMove(QGraphicsSceneMouseEvent* e);
    void angleModeMouseRelease(QGraphicsSceneMouseEvent* e);

    void scaleModeMousePress(QGraphicsSceneMouseEvent* e);
    void scaleModeMouseMove(QGraphicsSceneMouseEvent* e);
    void scaleModeMouseRelease(QGraphicsSceneMouseEvent* e);

    void rowEditMousePress(QGraphicsSceneMouseEvent* e);
    void rowEditMouseMove(QGraphicsSceneMouseEvent* e);
    void rowEditMouseRelease(QGraphicsSceneMouseEvent* e);
    
    void stitchModeMouseMove(QGraphicsSceneMouseEvent* e);
    void stitchModeMousePress(QGraphicsSceneMouseEvent* e);
    void stitchModeMouseRelease(QGraphicsSceneMouseEvent* e);

    QSizeF defaultSize() const { return mDefaultSize; }

    void updateStitchRenderer();

    void hideRowLines();

    
    
private:
    /**
     * Used in the mouse*Event()s to keep the mouse movements on the same cell.
     */
    QGraphicsItem* mCurItem;
    Cell* mCurCell;
    QPointF mCellStartPos;
    QPointF mLeftButtonDownPos;

    QPainterPath mSelectionPath;
    
    Indicator* mCurIndicator;
    
    /**
     * The difference between where the user clicked on the object and the (x,y) of the object.
     */
    QSizeF mDiff;
    qreal mOldAngle;

    QRubberBand* mRubberBand;
    QPointF mRubberBandStart;

    QMap<QGraphicsItem*, QPointF> mOldPositions;
    
    //Is the user moving an object.
    bool mMoving;
    bool mIsRubberband;
    bool mHasSelection;
    bool mSnapTo;
    
    EditMode mMode;
    
    QString mEditStitch;
    QColor mEditFgColor;
    QColor mEditBgColor;

    QPointF mOldScale;
    
    qreal mAngle;
    QPointF mPivotPt;
    QPointF mOrigin;

    //rows keeps track of the st order for individual rows;
    QList< QList<Cell*> > grid;
    
    qreal scenePosToAngle(QPointF pt);

    int mRowSpacing;
    QSizeF mDefaultSize;
    QString mDefaultStitch;
    
    /**
     *Hold the list of cells we're working with to create a new row.
     */
    QList<QGraphicsItem*> mRowSelection;
    
    QUndoStack mUndoStack;
    
    QList<Indicator*> mIndicators;

    Cell* mStartCell;
    Cell* mEndCell;
    Cell* mPreviousCell;

    /**
     *The line between the last cell and the mouse.
     */
    QGraphicsLineItem* mRowLine;

    /**
     *All lines connecting one cell to another cell for rows.
     */
    QList<QGraphicsLineItem*> mRowLines;
    
    QList<QGraphicsItem*> mDemoItems;

    QList<QGraphicsItemGroup*> mGroups;


/***
 *
 * Rounds specific functions:
 */
public:
    /**
     * Is there a visible center symbol?
     */
    bool showChartCenter();

    void createRoundsChart(int rows, int cols, QString stitch, QSizeF rowSize, int increaseBy);
    void createRow(int row, int columns, QString stitch);

    /**
     * Does the chart have a symbol at all?
     */
    bool hasChartCenter() { return (mCenterSymbol ? true : false); }
    QGraphicsItem* chartCenter() { return mCenterSymbol; }
    
public slots:
    void setShowChartCenter(bool state);

protected:
    void setCellPosition(int row, int column, Cell* c, int columns);  
    
private:
    QPointF calcPoint(double radius, double angleInDegrees, QPointF origin);

    QGraphicsItem* mCenterSymbol;
    bool mShowChartCenter;

public:
    bool showQuarterLines() { return mShowQuarterLines; }
    void setShowQuarterLines(bool state);
protected slots:
    void updateQuarterLines();
private:
    QGraphicsLineItem* mVerticalLine;
    QGraphicsLineItem* mHorizontalLine;
    QGraphicsLineItem* mAngleLine1;
    QGraphicsLineItem* mAngleLine2;
    bool mShowQuarterLines;

};

#endif //SCENE_H
