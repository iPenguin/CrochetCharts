/****************************************************************************\
 Copyright (c) 2011-2014 Stitch Works Software
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
#ifndef CHARTVIEW_H
#define CHARTVIEW_H

#include <QGraphicsView>

/**
 * The default view on the ChartScene.
 */
class ChartView : public QGraphicsView
{
    Q_OBJECT

public:
	enum SnapToGridType {
		Rows,
		Rounds
	};

public:
    ChartView(QWidget* parent = 0);
    ~ChartView();

    void zoomIn();
    void zoomOut();
    //zoom takes the mouseDelta which is usually +/- 120;
    void zoom(int mouseDelta);
    void zoomLevel(int percent);
	
	/**
	 * functions for snap to grid
	 */
	void setSnapToGrid(bool snapToGrid) { mSnapToGrid = snapToGrid; };
	bool snapToGrid() const { return mSnapToGrid; }
	
	void setSnapRows(unsigned int rows) { mRows = rows; }
	unsigned int snapRows() const { return mRows; }
	
	void setSnapColumns(unsigned int columns) { mColumns = columns; }
	unsigned int snapColumns() const { return mColumns; }
	
	void setSnapWidth(unsigned int width) { mWidth = width; }
	unsigned int snapWidth() const { return mWidth; }
	
	void setSnapHeight(unsigned int height) { mHeight = height; }
	unsigned int snapHeight() const { return mHeight; }
	
	void setSnapPos(QPointF pos) { mPos = pos; }
	QPointF snapPos() const { return mPos; }
	
	void setSnapType(SnapToGridType snapType) { mSnapType = snapType; }
	SnapToGridType snapType() const { return mSnapType; }
	
	QPointF snapPosition(QPointF pos) const;

signals:
    void scrollBarChanged(int dx, int dy);
    void zoomLevelChanged(int percent);

protected:
    void mouseMoveEvent(QMouseEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);
    
private:
	/**
	 * Variables for snap to grid.
	 */
	bool mSnapToGrid;
	unsigned int mRows;
	unsigned int mColumns;
	unsigned int mWidth;
	unsigned int mHeight;
	QPointF mPos;
	SnapToGridType mSnapType;

};

#endif //CHARTVIEW_H
