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
public:

    enum EditMode {
        StitchMode = 10, //place stitches on the chart.
        ColorMode = 11,       //place colors behind stitches.
        AngleMode = 14,       //adjust the angle of the
        StretchMode = 15,       //stretch the stitches.
        IndicatorMode = 16
    };

    enum ChartStyle {
        Rows = 100, // flat chart rows calc'ed based on grid.
        Rounds,     // round chart rows clac'ed with trig.
        Blank       // start w/no sts and allow anything.
    };
    
    Scene(QObject* parent = 0);
    ~Scene();

    virtual void createRow(int row, int columns, QString stitch) = 0;

    virtual void appendCell(int row, CrochetCell* c) = 0;

    virtual void addCell(CrochetCell* c);
    
    int rowCount();
    int columnCount(int row);
    int maxColumnCount();
    
    virtual void removeCell(CrochetCell* c) = 0;

    virtual void createChart(int rows, int cols, QString stitch, QSizeF rowSize) = 0;

    void setEditMode(EditMode mode) { mMode = mode; }
    EditMode editMode() { return mMode; }

    void setEditStitch(QString stitch) { mEditStitch = stitch; }

    void setEditFgColor(QColor color) { mEditFgColor = color; }
    void setEditBgColor(QColor color) { mEditBgColor = color; }

    QUndoStack* undoStack() { return &mUndoStack; }

    void addIndicator(Indicator* i);
    void removeIndicator(Indicator* i);
    
    QStringList modes();
    
public slots:
    
    void updateRubberBand(int dx, int dy);
    
signals:
    void stitchChanged(QString oldSt, QString newSt);
    void colorChanged(QString oldColor, QString newColor);
    
protected:
/*
    virtual void    contextMenuEvent ( QGraphicsSceneContextMenuEvent * contextMenuEvent )
    virtual void    helpEvent ( QGraphicsSceneHelpEvent * helpEvent )
*/
    void keyReleaseEvent(QKeyEvent* keyEvent);
    void mouseMoveEvent(QGraphicsSceneMouseEvent* e);
    void mousePressEvent(QGraphicsSceneMouseEvent* e);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* e);

    /**
     * find the x,y positions on the grid for a given cell;
     * return QPoint(column, row);
     */
    QPoint findGridPosition(CrochetCell* c);

    QList<Indicator*> indicators() { return mIndicators; }
        
    void initDemoBackground();

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

    virtual void stitchModeMouseMove(QGraphicsSceneMouseEvent* e) { Q_UNUSED(e); }
    virtual void stitchModeMousePress(QGraphicsSceneMouseEvent* e) { Q_UNUSED(e); }
    virtual void stitchModeMouseRelease(QGraphicsSceneMouseEvent* e) { Q_UNUSED(e); }
    
    QSizeF defaultSize() const { return mDefaultSize; }

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

    //The grid just keeps track of the sts in each row so they can be converted to instructions.
    QList<QList<CrochetCell*> > grid;
    
private:
    qreal scenePosToAngle(QPointF pt);

    int mRowSpacing;
    QSizeF mDefaultSize;
        
    
    QUndoStack mUndoStack;
    
    QList<Indicator*> mIndicators;

    QList<QGraphicsItem*> mDemoItems;
};

#endif //SCENE_H
