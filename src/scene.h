/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef SCENE_H
#define SCENE_H

#include <QGraphicsScene>

#include "crochetcell.h"

#include <QUndoStack>
#include <QRubberBand>

#include "indicator.h"

class QKeyEvent;

class Scene : public QGraphicsScene
{
    Q_OBJECT
    friend class SaveFile;
    friend class SaveThread;
    friend class SetCellStitch;
    friend class SetCellColor;
    friend class SetCellRotation;
    friend class AddCell;
    friend class RemoveCell;
    friend class RowEditDialog;
    friend class TextView;
public:

    enum EditMode {
        StitchEdit = 10, //place stitches on the chart.
        ColorEdit = 11,       //place colors behind stitches.
        RowEdit = 12,
        RotationEdit = 14,       //adjust the angle of the
        ScaleEdit = 15,       //stretch the stitches.
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
    CrochetCell* cell(int row, int column);
    //convert x,y to rows, columns.
    CrochetCell* cell(QPoint position);

    virtual void addCell(CrochetCell* c);

    /**
     * find the x,y positions on the grid for a given cell;
     * return QPoint(column, row);
     * if return = -1,-1 isVoid.
     */
    QPoint indexOf(CrochetCell* c);
    
    void removeCell(CrochetCell* c);
    /**
     * This function removes a cell from the 'grid'. if the row is empty it removes the row too.
     */
    void removeFromRows(CrochetCell* c);
    
    int rowCount();
    int columnCount(int row);
    int maxColumnCount();

    void setEditMode(EditMode mode) { mMode = mode; if(mode != Scene::RowEdit) hideRowLines(); }
    EditMode editMode() { return mMode; }

    void setEditStitch(QString stitch) { mEditStitch = stitch; }

    void setEditFgColor(QColor color) { mEditFgColor = color; }
    void setEditBgColor(QColor color) { mEditBgColor = color; }

    QUndoStack* undoStack() { return &mUndoStack; }

    void addIndicator(Indicator* i);
    void removeIndicator(Indicator* i);
    
    QStringList modes();

    void moveRowUp(int row);
    void moveRowDown(int row);

    void removeRow(int row);

    void alignSelection(int alignmentStyle);
    void distributeSelection(int distributionStyle);
    void arrangeGrid(QSize grid, QSize alignment, QSize spacing, bool useSelection);
    
    void copy();
    void cut();
    void paste();
    
public slots:
    void createRow();
    void updateRow(int row);
    
    /**
     * highlight (select) all the stitches in row @param row.
     */
    void highlightRow(int row);
    void drawRowLines(int row);

    void updateRubberBand(int dx, int dy);
    
signals:
    void stitchChanged(QString oldSt, QString newSt);
    void colorChanged(QString oldColor, QString newColor);

    void rowSelected();
    
protected:
/*
    virtual void    contextMenuEvent ( QGraphicsSceneContextMenuEvent * contextMenuEvent )
    virtual void    helpEvent ( QGraphicsSceneHelpEvent * helpEvent )
*/
    void keyReleaseEvent(QKeyEvent* keyEvent);
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

protected:
    void colorModeMouseMove(QGraphicsSceneMouseEvent* e);
    void colorModeMouseRelease(QGraphicsSceneMouseEvent* e);
    
    void indicatorModeMouseMove(QGraphicsSceneMouseEvent* e);
    void indicatorModeMouseRelease(QGraphicsSceneMouseEvent* e);

    void angleModeMousePress(QGraphicsSceneMouseEvent* e);
    void angleModeMouseMove(QGraphicsSceneMouseEvent* e);
    void angleModeMouseRelease(QGraphicsSceneMouseEvent* e);

    void stretchModeMousePress(QGraphicsSceneMouseEvent* e);
    void stretchModeMouseMove(QGraphicsSceneMouseEvent* e);
    void stretchModeMouseRelease(QGraphicsSceneMouseEvent* e);

    void rowEditMousePress(QGraphicsSceneMouseEvent* e);
    void rowEditMouseMove(QGraphicsSceneMouseEvent* e);
    void rowEditMouseRelease(QGraphicsSceneMouseEvent* e);
    
    virtual void stitchModeMouseMove(QGraphicsSceneMouseEvent* e) { Q_UNUSED(e); }
    virtual void stitchModeMousePress(QGraphicsSceneMouseEvent* e) { Q_UNUSED(e); }
    void stitchModeMouseRelease(QGraphicsSceneMouseEvent* e);

    QSizeF defaultSize() const { return mDefaultSize; }

    void updateStitchRenderer();

    void hideRowLines();
    
protected:
    /**
     * Used in the mouse*Event()s to keep the mouse movements on the same cell.
     */
    CrochetCell* mCurCell;
    QPointF mCellStartPos;
    QPointF mLeftButtonDownPos;
    
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
    
    EditMode mMode;
    
    QString mEditStitch;
    QColor mEditFgColor;
    QColor mEditBgColor;

    qreal mScale;
    qreal mOldScale;
    qreal mCurScale;

    qreal mAngle;
    QPointF mPivotPt;
    QPointF mOrigin;

    //rows keeps track of the st order for individual rows;
    QList< QList<CrochetCell*> > grid;
    
private:
    qreal scenePosToAngle(QPointF pt);

    int mRowSpacing;
    QSizeF mDefaultSize;

    QList<QGraphicsItem*> mRowSelection;
    
    QUndoStack mUndoStack;
    
    QList<Indicator*> mIndicators;

    CrochetCell* mStartCell;
    CrochetCell* mEndCell;
    CrochetCell* mPreviousCell;
    QGraphicsLineItem* mRowLine;
    QList<QGraphicsLineItem*> mRowLines;
    
    QList<QGraphicsItem*> mDemoItems;
};

#endif //SCENE_H
