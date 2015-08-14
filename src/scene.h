/****************************************************************************\
 C opyright (c) 2010-2014 Stitch Works Software                                 *
 Brian C. Milco <bcmilco@gmail.com>

 This file is part of Crochet Charts.

 Crochet Charts is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Crochet Charts is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Crochet Charts. If not, see <http://www.gnu.org/licenses/>.

 \****************************************************************************/
#ifndef SCENE_H
#define SCENE_H

#include <QGraphicsScene>

#include "cell.h"

#include <QHash>
#include <QUndoStack>
#include <QRubberBand>

#include "chartLayer.h"
#include "indicator.h"
#include "itemgroup.h"

class Guidelines {
public:
    explicit Guidelines();

    QString type() const { return mType; }
    void setType(QString type) { mType = type; }

    int rows() const { return mRows; }
    void setRows(int rows) { mRows = rows; }
    int columns() const { return mColumns; }
    void setColumns(int columns) { mColumns = columns; }

    int cellHeight() const { return mCellHeight; }
    void setCellHeight(int cellHeight) { mCellHeight = cellHeight; }
    int cellWidth() const { return mCellWidth; }
    void setCellWidth(int cellWidth) { mCellWidth = cellWidth; }

    bool operator==(const Guidelines &other) const;
    bool operator!=(const Guidelines &other) const;

private:
    QString mType;

    int mRows;
    int mColumns;

    int mCellHeight;
    int mCellWidth;
};

Q_CORE_EXPORT QDataStream & operator<< ( QDataStream & stream, Guidelines & guidelines );
QDebug operator<<(QDebug d, Guidelines & guidelines);

Q_DECLARE_METATYPE(Guidelines)

class IndicatorProperties
{
public:
    explicit IndicatorProperties()
    {
    }

    QString style() { return mStyle; }
    void setStyle(QString style) { mStyle = style; }

    QString html() { return mHtml; }
    void setHtml(QString html) { mHtml = html; }
	
	QFont font() { return mFont; }
	void setFont(const QFont& font) { mFont = font; }
	
	int size() { return mSize; }
	void setSize(int size) { mSize = size; }
	
private:
	QFont mFont;
	int mSize;
    QString mHtml;
    QString mStyle;
};

QDebug operator<<(QDebug d, IndicatorProperties & properties);
Q_DECLARE_METATYPE(IndicatorProperties)

class QKeyEvent;

class Scene : public QGraphicsScene
{
    Q_OBJECT
    friend class FileFactory;
    friend class File_v1;
    friend class File_v2;
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
    
    Scene(QObject *parent = 0);
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
    QPoint indexOf(Cell *c);
    
    int rowCount();
    int columnCount(int row);
    int maxColumnCount();
	QList<ChartLayer*> layers();

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
	void copy(int direction);
    void rotate(qreal degrees);
	void resizeScene(QRectF sceneRect);

    void group();
    void ungroup();

    void createRowsChart(int rows, int cols, QString defStitch, QSizeF rowSize);
    void createBlankChart();

    void addItem(QGraphicsItem *item);
    void removeItem(QGraphicsItem *item);

	//returns the current layer and creates a new layer if no layer is currently selected
	ChartLayer* getCurrentLayer();

    /**
     * Add a row of stitches to the grid.
     * If append == false, use the rowPos to insert the row into the grid.
     */
    void gridAddRow(QList< Cell* > row, bool append = true, int before = 0);

    /**
     * @brief propertiesUpdate - updates the properties of all selected items.
     * @param property - name of the property to update
     * @param newValue - new value to set.
     *
     * Values for property include: Angle, ScaleX, ScaleY, Stitch, ChartCenter, Guidelines, Deletes
     */
    void propertiesUpdate(QString property, QVariant newValue);

    void updateDefaultStitchColor(QColor originalColor, QColor newColor);

public slots:    
    void copy();
    void cut();
    void paste();
    void deleteSelection();
    
protected:
    void copyRecursively(QDataStream &stream, QList<QGraphicsItem*> items);
    void pasteRecursively(QDataStream &stream, QList<QGraphicsItem*> *group);

    /**
     * This function removes a cell from the 'grid'. if the row is empty it removes the row too.
     */
    void removeFromRows(Cell *c);

    void updateSceneRect();
    
public slots:
	/**
	 * layer manipulation functions
	 */
	void addLayer(const QString& layer);
	void addLayer(const QString& layer, unsigned int uid);
	void removeSelectedLayer();
	void selectLayer(unsigned int uid);
	
	/**
	 * row manipulation functions
	 */
    void createRow();
    void updateRow(int row);
    
    /**
     * highlight (select) all the stitches in row @param row.
     */
    void highlightRow(int row);

    /**
     * @brief drawRowLines - draw the lines between stitches that make up a row.
     * @param row - the row to draw the lines for.
     */
    void drawRowLines(int row);

    void highlightIndicators(bool state);

    void updateRubberBand(int dx, int dy);

    void editorLostFocus(Indicator *item);
    void editorGotFocus(Indicator *item);
    
signals:
    void stitchChanged(QString oldSt, QString newSt);
    void colorChanged(QString oldColor, QString newColor);
	void layersChanged(QList<ChartLayer*>& layers);

    void rowSelected();

    //When ever a row is edited emit this signal.
    void rowEdited(bool state);

    void guidelinesUpdated(Guidelines guidelines);
    
protected:
//    virtual void    helpEvent ( QGraphicsSceneHelpEvent * helpEvent )
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *e);

    void keyReleaseEvent(QKeyEvent *keyEvent);
    void keyPressEvent(QKeyEvent *keyEvent);
    
    void stitchModeKeyRelease(QKeyEvent *keyEvent);
    void angleModeKeyRelease(QKeyEvent *keyEvent);
    void scaleModeKeyRelease(QKeyEvent *keyEvent);
    
    void mouseMoveEvent(QGraphicsSceneMouseEvent *e);
    void mousePressEvent(QGraphicsSceneMouseEvent *e);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *e);

    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *e);
    
    QList<Indicator*> indicators() { return mIndicators; }

    /**
     * vertical:   
     *             0 - don't change alignment
     *             1 - align top
     *             2 - align center v
     *             3 - align bottom
     * horizontal: 
     *             0 - don't change alignment
     *             1 - align left
     *             2 - align center h
     *             3 - align right
     */
    void align(int vertical, int horizontal);
    /**
     * vertical:   
     *             0 - don't change distribution
     *             1 - distribute top
     *             2 - distribute center v
     *             3 - distribute bottom
     * horizontal: 
     *             0 - don't change distribution
     *             1 - distribute left
     *             2 - distribute center h
     *             3 - distribute right
     */
    void distribute(int vertical, int horizontal);
    
    void alignToPath();
    void distributeToPath();

    /**
     * To distribute the stitches evenly we first need to know the sort order of the stitches
     * we're about to work on, based on which edge we're looking at.
     * 
     * sortEdge: 1 = left; 2 = center; 3 = right
     * bad values for sortEdge will result in a left align sort.
     */
    QList<QGraphicsItem*> sortItemsHorizontally(QList<QGraphicsItem*> unsortedItems, int sortEdge = 0);
    
    /**
     * To distribute the stitches evenly we first need to know the sort order of the stitches
     * we're about to work on, based on which edge we're looking at.
     * 
     * @sortEdge: 1 = top; 2 = center; 3 = bottom
     * bad values for @sortEdge will result in a top align sort.
     */
    QList< QGraphicsItem* > sortItemsVertically(QList< QGraphicsItem* > unsortedItems, int sortEdge = 0);
    
    /**
     * Return a sceneBoundingRect for the selected items.
     * @vertical and @horizontal determine what kind of bounding rect will be returned.
     * 
     * vertical:   
     *             0 - use extremes for top and bottom
     *             1 - use the tops of all stitches
     *             2 - use the centers of all stitches
     *             3 - use the bottoms of all stitches
     * horizontal: 
     *             0 - use extremes for left and right
     *             1 - use the left edges of all stitches
     *             2 - use the centers of all stitches
     *             3 - use the right edges of all stitches
     */
    QRectF selectionBoundingRect(QList< QGraphicsItem* > items, int vertical, int horizontal);

public:
    ItemGroup* group(QList<QGraphicsItem*> items, ItemGroup *g = 0);
    void ungroup(ItemGroup *group);

    void addToGroup(int groupNumber, QGraphicsItem *i);

    QRectF selectedItemsBoundingRect(QList<QGraphicsItem*> items);

    /**
     * This function overrides the itemsBoundingRect().
     */
    QRectF itemsBoundingRect();

    void render(QPainter *painter, const QRectF &target = QRectF(), const QRectF &source = QRectF(),
                Qt::AspectRatioMode aspectRatioMode = Qt::KeepAspectRatio);
	void drawBackground(QPainter * painter, const QRectF & rect);
	void setBackground(bool enabled);
protected:
    void colorModeMouseMove(QGraphicsSceneMouseEvent *e);
    void colorModeMouseRelease(QGraphicsSceneMouseEvent *e);

    void indicatorModeMouseMove(QGraphicsSceneMouseEvent *e);
    void indicatorModeMouseRelease(QGraphicsSceneMouseEvent *e);

    void angleModeMousePress(QGraphicsSceneMouseEvent *e);
    void angleModeMouseMove(QGraphicsSceneMouseEvent *e);
    void angleModeMouseRelease(QGraphicsSceneMouseEvent *e);

    void scaleModeMousePress(QGraphicsSceneMouseEvent *e);
    void scaleModeMouseMove(QGraphicsSceneMouseEvent *e);
    void scaleModeMouseRelease(QGraphicsSceneMouseEvent *e);

    void rowEditMousePress(QGraphicsSceneMouseEvent *e);
    void rowEditMouseMove(QGraphicsSceneMouseEvent *e);
    void rowEditMouseRelease(QGraphicsSceneMouseEvent *e);
    
    void stitchModeMouseMove(QGraphicsSceneMouseEvent *e);
    void stitchModeMousePress(QGraphicsSceneMouseEvent *e);
    void stitchModeMouseRelease(QGraphicsSceneMouseEvent *e);

    QSizeF defaultSize() const { return mDefaultSize; }

    void updateStitchRenderer();

    void hideRowLines();

private:
    /**
     * Used in the mouse*Event()s to keep the mouse movements on the same cell.
     */
    QGraphicsItem *mCurItem;
    QPointF mCellStartPos;
    QPointF mLeftButtonDownPos;

    QPainterPath mSelectionPath;
    
    Indicator *mCurIndicator;
    
    /**
     * The difference between where the user clicked on the object and the (x,y) of the object.
     */
    QSizeF mDiff;
    qreal mOldAngle;

    QRubberBand *mRubberBand;
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
    QTransform mOldTransform;
    QSizeF mOldSize;

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

    Cell *mStartCell;
    Cell *mEndCell;
    Cell *mPreviousCell;

    /**
     *The line between the last cell and the mouse.
     */
    QGraphicsLineItem *mRowLine;

    /**
     *All lines connecting one cell to another cell for rows.
     */
    QList<QGraphicsLineItem*> mRowLines;
    
    QList<ItemGroup*> mGroups;
	
	QHash<unsigned int, ChartLayer*> mLayers;
	ChartLayer* mSelectedLayer;
	
	bool mbackgroundIsEnabled;
	
/***
 * Generic private functions
 ***/
private:
	QGraphicsItem* mirror_rec(QGraphicsItem* item, QPointF displacement, QRectF selectionRect, bool flipX, bool flipY);
	QGraphicsItem* copy_rec(QGraphicsItem* item, QPointF displacement);
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
    void setCellPosition(int row, int column, Cell *c, int columns);
    
private:
    QPointF calcPoint(double radius, double angleInDegrees, QPointF origin);

    QGraphicsItem *mCenterSymbol;
    bool mShowChartCenter;

public:
    void setGuidelinesType(QString guides);
    Guidelines guidelines() { return mGuidelines; }

    void replaceStitches(QString original, QString replacement);

    /**
     * @brief replaceColor - repleace the original color with the replacement color
     * @param original - color to look for on the chart
     * @param replacement - color to replace it with
     * @param selection - foreground = 1, background = 2, or foreground & background = 3.
     */
    void replaceColor(QColor original, QColor replacement, int selection);

protected slots:
    /**
     * @brief updateGuidelines - draw the guidelines
     */
    void updateGuidelines();

private:
    /**
     * @brief mGuidelinesLines - Hold all the lines that make up the grid
     */
    QList<QGraphicsItem*> mGuidelinesLines;

    /**
     * @brief mGuidelines - Hold the settings that are used to generate a grid background
     */
    Guidelines mGuidelines;
};

#endif //SCENE_H
